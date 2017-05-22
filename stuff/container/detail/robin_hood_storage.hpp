#pragma once

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>

#include <boost/iterator/iterator_facade.hpp>
#include <stuff/assert.hpp>
#include <stuff/container/detail/hash_table_growth_policy.hpp>
#include <stuff/container/selectors.hpp>
#include <stuff/meta/named_template_params.hpp>

namespace stuff::container::detail {

DECLARE_VALUE_SELECTOR(max_size, std::size_t);

template <typename T, typename HashValue, typename Metadata, typename Allocator>
class raw_storage {
    struct aux {
        Metadata metadata;
        HashValue hash;
    };
    using aux_allocator =
        typename std::allocator_traits<Allocator>::template rebind_alloc<aux>;

    aux* aux_ = nullptr;
    T* values_ = nullptr;
    std::size_t capacity_ = 0;
    Allocator alloc_;

    void allocate_all() {
        if (capacity_ > 0) {
            aux_allocator aux_alloc{alloc_};
            aux_ = aux_alloc.allocate(capacity_);
            std::memset(static_cast<void*>(aux_), 0, capacity_ * sizeof(aux));
            values_ = alloc_.allocate(capacity_);
        }
    }

    void deallocate_all() {
        if (capacity_ > 0) {
            alloc_.deallocate(values_, capacity_);
            aux_allocator aux_alloc{alloc_};
            aux_alloc.deallocate(aux_, capacity_);
        }
    }

public:
    explicit raw_storage(const Allocator& alloc = Allocator{}) : raw_storage(0, alloc) {}
    raw_storage(raw_storage&&) = default;

    explicit raw_storage(std::size_t capacity, const Allocator& alloc = Allocator{})
        : capacity_(capacity), alloc_(alloc) {
        allocate_all();
    }

    ~raw_storage() { deallocate_all(); }

    raw_storage& operator=(raw_storage&& other) {
        std::swap(aux_, other.aux_);
        std::swap(values_, other.values_);
        std::swap(capacity_, other.capacity_);
        std::swap(alloc_, other.alloc_);
        return *this;
    }

    HashValue hash_at(std::size_t pos) const noexcept { return aux_[pos].hash; };

    const Metadata& metadata_at(std::size_t pos) const noexcept {
        return aux_[pos].metadata;
    }
    Metadata& metadata_at(std::size_t pos) noexcept { return aux_[pos].metadata; }

    const T& value_at(std::size_t pos) const noexcept { return values_[pos]; }
    T& value_at(std::size_t pos) noexcept { return values_[pos]; }

    const Allocator& allocator() const noexcept { return alloc_; }

    std::size_t capacity() const noexcept { return capacity_; }

    void destroy(std::size_t pos) noexcept { values_[pos].~T(); }

    void put(std::size_t pos, T&& value, HashValue hash, Metadata metadata) {
        ::new (static_cast<void*>(std::addressof(values_[pos]))) T(std::move(value));
        aux_[pos].metadata = metadata;
        aux_[pos].hash = hash;
    }

    std::pair<T, HashValue> replace(std::size_t pos, T&& value, HashValue hash,
                                    Metadata metadata) {
        const auto current_hash = hash_at(pos);
        std::swap(value, values_[pos]);
        aux_[pos].metadata = metadata;
        aux_[pos].hash = hash;
        return {std::move(value), current_hash};
    }

    friend void swap(raw_storage& l, raw_storage& r) {
        std::swap(l.aux_, r.aux_);
        std::swap(l.values_, r.values_);
        std::swap(l.capacity_, r.capacity_);
        std::swap(l.alloc_, r.alloc_);
    }
};

template <typename T, typename Metadata, typename Allocator>
class raw_storage_no_hash {
    using aux = Metadata;
    using aux_allocator =
        typename std::allocator_traits<Allocator>::template rebind_alloc<aux>;

    aux* aux_ = nullptr;
    T* values_ = nullptr;
    std::size_t capacity_ = 0;
    Allocator alloc_;

    void allocate_all() {
        if (capacity_ > 0) {
            aux_allocator aux_alloc{alloc_};
            aux_ = aux_alloc.allocate(capacity_);
            std::memset(static_cast<void*>(aux_), 0, capacity_ * sizeof(aux));
            values_ = alloc_.allocate(capacity_);
        }
    }

    void deallocate_all() {
        if (capacity_ > 0) {
            alloc_.deallocate(values_, capacity_);
            aux_allocator aux_alloc{alloc_};
            aux_alloc.deallocate(aux_, capacity_);
        }
    }

public:
    explicit raw_storage_no_hash(const Allocator& alloc = Allocator{})
        : raw_storage_no_hash(0, alloc) {}
    raw_storage_no_hash(raw_storage_no_hash&&) = default;

    explicit raw_storage_no_hash(std::size_t capacity,
                                 const Allocator& alloc = Allocator{})
        : capacity_(capacity), alloc_(alloc) {
        allocate_all();
    }

    ~raw_storage_no_hash() { deallocate_all(); }

    raw_storage_no_hash& operator=(raw_storage_no_hash&& other) {
        std::swap(aux_, other.aux_);
        std::swap(values_, other.values_);
        std::swap(capacity_, other.capacity_);
        std::swap(alloc_, other.alloc_);
        return *this;
    }

    const Metadata& metadata_at(std::size_t pos) const noexcept { return aux_[pos]; }
    Metadata& metadata_at(std::size_t pos) noexcept { return aux_[pos]; }

    const T& value_at(std::size_t pos) const noexcept { return values_[pos]; }
    T& value_at(std::size_t pos) noexcept { return values_[pos]; }

    const Allocator& allocator() const noexcept { return alloc_; }

    std::size_t capacity() const noexcept { return capacity_; }

    void destroy(std::size_t pos) noexcept { values_[pos].~T(); }

    void put(std::size_t pos, T&& value, Metadata metadata) {
        ::new (static_cast<void*>(std::addressof(values_[pos]))) T(std::move(value));
        aux_[pos] = metadata;
    }

    T replace(std::size_t pos, T&& value, Metadata metadata) {
        std::swap(value, values_[pos]);
        aux_[pos] = metadata;
        return std::move(value);
    }

    friend void swap(raw_storage_no_hash& l, raw_storage_no_hash& r) {
        std::swap(l.aux_, r.aux_);
        std::swap(l.values_, r.values_);
        std::swap(l.capacity_, r.capacity_);
        std::swap(l.alloc_, r.alloc_);
    }
};

constexpr size_t upper_power_of_2(std::size_t n) {
    if (n == 0 || n == 1) {
        return 1;
    } else {
        return 2 * upper_power_of_2((n + 1) / 2);
    }
}

template <std::size_t max_size>
struct bucket_metadata {
    enum class deleted : uint64_t { YES = 1, NO = 0 };

    deleted deleted : 1;
    std::uint64_t size : max_size;
    std::uint64_t distance : 64 - 1 - max_size;

    bucket_metadata(std::size_t distance, std::size_t size = 0,
                    enum deleted deleted = deleted::NO)
        : deleted(deleted), size(size), distance(distance) {}

    static bucket_metadata vacant() { return {0, 0, deleted::NO}; }
};

template <typename T, typename Hash, typename Equal, typename Allocator,
          typename... Params>
class robin_hood_storage {
    using growth_policy = weird_powers_of_two_growth_policy;
    static constexpr std::size_t max_size_ =
        upper_power_of_2(meta::get_v<max_size<1ull << 37>, Params...>);
    static constexpr bool store_hash_ =
        meta::get_v<container::store_hash<true>, Params...>;

    using hash_type = std::uint32_t;
    using metadata = bucket_metadata<37>;
    using raw_storage_type =
        std::conditional_t<store_hash_, raw_storage<T, hash_type, metadata, Allocator>,
                           raw_storage_no_hash<T, metadata, Allocator>>;

    raw_storage_type raw_storage_;

    typename growth_policy::reducer to_position_;
    Hash hash_;
    Equal equal_;
    std::size_t size_ = 0;

public:
    struct iterator
        : public boost::iterator_facade<iterator, const T, boost::forward_traversal_tag> {
        iterator(const iterator&) = default;
        iterator(iterator&&) = default;

        iterator& operator=(const iterator&) = default;
        iterator& operator=(iterator&&) = default;

    private:
        std::size_t pos_;
        const robin_hood_storage* parent_;

        friend class robin_hood_storage;
        friend class boost::iterator_core_access;

        iterator(std::size_t pos, const robin_hood_storage* parent)
            : pos_(pos), parent_(parent) {}

        const T& dereference() const noexcept { return parent_->value_at(pos_); }

        bool equal(const iterator& other) const noexcept {
            return pos_ == other.pos_ && parent_ == other.parent_;
        }

        void increment() noexcept {
            while (true) {
                ++pos_;
                if (pos_ == parent_->capacity() ||
                    (!parent_->deleted_at(pos_) && !parent_->vacant_at(pos_))) {
                    break;
                }
            }
        }
    };

    iterator iterator_for(std::size_t pos) const noexcept { return iterator{pos, this}; }

private:
    bool deleted_at(std::size_t pos) const noexcept {
        return raw_storage_.metadata_at(pos).deleted == metadata::deleted::YES;
    }

    std::uint32_t distance_at(std::size_t pos) const noexcept {
        return raw_storage_.metadata_at(pos).distance;
    }

    void erase_at(std::size_t pos) {
        raw_storage_.destroy(pos);
        raw_storage_.metadata_at(pos).deleted = metadata::deleted::YES;
        --size_;
    }

    template <typename V>
    hash_type folded_hash(const V& value) const noexcept(noexcept(hash_(value))) {
        const auto h = hash_(value);
        // return (h & 0x00000000FFFFFFFF) ^ (h >> 32);
        return static_cast<hash_type>(h);
    }

    void grow() {
        const uint64_t growth_factor = 2;
        const auto old_capacity = raw_storage_.capacity();
        std::size_t new_capacity;
        std::tie(new_capacity, to_position_) =
            growth_policy::get(std::max(capacity() * growth_factor, 1llu));
        raw_storage_type old_raw_storage(new_capacity, raw_storage_.allocator());
        swap(old_raw_storage, raw_storage_);

        size_ = 0;
        for (size_t pos = 0; pos < old_capacity; ++pos) {
            const auto& md = old_raw_storage.metadata_at(pos);
            // if (metadata.vacant == metadata_type::vacant::NO &&
            if (*reinterpret_cast<const std::uint64_t*>(&md) != 0 &&
                md.deleted == metadata::deleted::NO) {
                if
                    constexpr(store_hash_) {
                        insert_impl(std::move(old_raw_storage.value_at(pos)),
                                    old_raw_storage.hash_at(pos));
                    }
                else {
                    insert_impl(std::move(old_raw_storage.value_at(pos)));
                }
            }
        }
    }

    void put(std::size_t pos, T&& value, hash_type hash, metadata metadata) {
        if
            constexpr(store_hash_) {
                raw_storage_.put(pos, std::move(value), hash, metadata);
            }
        else {
            raw_storage_.put(pos, std::move(value), metadata);
        }
    }

    decltype(auto) replace(std::size_t pos, T&& value, hash_type hash,
                           metadata metadata) {
        if
            constexpr(store_hash_) {
                return raw_storage_.replace(pos, std::move(value), hash, metadata);
            }
        else {
            return raw_storage_.replace(pos, std::move(value), metadata);
        }
    }

    template <typename Value>
    std::pair<iterator, bool> insert_impl(Value&& value) {
        const auto hash = folded_hash(value);
        // Creating copy of value only if necessary
        return insert_impl(std::forward<Value>(value), hash);
    }

    std::pair<iterator, bool> insert_impl(T&& value, hash_type hash) {
        auto pos = position_for_hash(hash);
        auto& bkt_metadata = raw_storage_.metadata_at(pos);
        std::uint32_t distance = 0;

        while (true) {
            if (vacant_at(pos)) {
                put(pos, std::move(value), hash, metadata{distance});
                ++size_;
                ++bkt_metadata.size;
                return {iterator_for(pos), true};
            }

            const bool deleted_at_pos = deleted_at(pos);
            if (!deleted_at_pos && equal_(raw_storage_.value_at(pos), value)) {
                return {iterator_for(pos), false};
            }

            const auto distance_at_pos = distance_at(pos);
            if (distance_at_pos < distance) {
                if (!deleted_at_pos) {
                    std::tie(value, hash) = raw_storage_.replace(
                        pos, std::move(value), hash,
                        metadata{distance, raw_storage_.metadata_at(pos).size});
                    continue_insertion(std::move(value), hash, next_position(pos),
                                       distance_at_pos + 1);
                } else {
                    put(pos, std::move(value), hash,
                        metadata{distance, raw_storage_.metadata_at(pos).size});
                }
                ++size_;
                ++bkt_metadata.size;
                return {iterator_for(pos), true};
            }
            ++distance;
            pos = next_position(pos);
        }
    }

    std::pair<iterator, bool> insert_impl(const T& value, hash_type hash) {
        auto pos = position_for_hash(hash);
        auto& bkt_metadata = raw_storage_.metadata_at(pos);
        std::uint32_t distance = 0;

        while (true) {
            if (vacant_at(pos)) {
                put(pos, T(value), hash, metadata{distance});
                ++size_;
                ++bkt_metadata.size;
                return {iterator_for(pos), true};
            }

            const bool deleted_at_pos = deleted_at(pos);
            if (!deleted_at_pos && equal_(raw_storage_.value_at(pos), value)) {
                return {iterator_for(pos), false};
            }

            const auto distance_at_pos = distance_at(pos);
            if (distance_at_pos < distance) {
                if (!deleted_at_pos) {
                    if
                        constexpr(store_hash_) {
                            T value_at_pos;
                            std::tie(value_at_pos, hash) = raw_storage_.replace(
                                pos, T(value), hash,
                                metadata{distance, raw_storage_.metadata_at(pos).size});
                            continue_insertion(std::move(value_at_pos), hash,
                                               next_position(pos), distance_at_pos + 1);
                        }
                    else {
                        T value_at_pos = raw_storage_.replace(
                            pos, T(value),
                            metadata{distance, raw_storage_.metadata_at(pos).size});
                        continue_insertion(std::move(value_at_pos), hash,
                                           next_position(pos), distance_at_pos + 1);
                    }
                } else {
                    put(pos, T(value), hash,
                        metadata{distance, raw_storage_.metadata_at(pos).size});
                }
                ++size_;
                ++bkt_metadata.size;
                return {iterator_for(pos), true};
            }
            ++distance;
            pos = next_position(pos);
        }
    }

    void continue_insertion(T&& value, hash_type hash, std::size_t pos,
                            std::uint32_t distance) {
        while (true) {
            if (vacant_at(pos)) {
                put(pos, std::move(value), hash, metadata{distance});
                break;
            }

            const auto distance_at_pos = distance_at(pos);
            if (distance_at_pos < distance) {
                if (deleted_at(pos)) {
                    put(pos, std::move(value), hash,
                        metadata{distance, raw_storage_.metadata_at(pos).size});
                    break;
                }
                if
                    constexpr(store_hash_) {
                        std::tie(value, hash) = replace(
                            pos, std::move(value), hash,
                            metadata{distance, raw_storage_.metadata_at(pos).size});
                    }
                else {
                    value =
                        replace(pos, std::move(value), hash,
                                metadata{distance, raw_storage_.metadata_at(pos).size});
                }
                distance = distance_at_pos;
            }
            ++distance;
            pos = next_position(pos);
        }
    }

    std::size_t next_position(std::uint64_t pos) const noexcept {
        ++pos;
        return to_position_(pos);
    }

    std::size_t position_for_hash(hash_type h) const noexcept { return to_position_(h); }

    template <typename V>
    std::size_t search(const V& value) const
        noexcept(noexcept(folded_hash(value) && equal_(value, value_at(0)))) {
        constexpr bool use_hash_for_comparison = sizeof(T) > 8;

        if (empty()) {
            return capacity();
        }
        const auto hash = folded_hash(value);
        auto pos = position_for_hash(hash);
        std::uint32_t distance = 0;
        while (true) {
            while (pos < capacity()) {
                if (vacant_at(pos) || distance > distance_at(pos)) {
                    return capacity();
                }

                if
                    constexpr(store_hash_ && use_hash_for_comparison) {
                        if (/*distance_at(pos) == distance &&*/ !deleted_at(pos) &&
                            hash == raw_storage_.hash_at(pos) &&
                            equal_(value, value_at(pos))) {
                            return pos;
                        }
                    }
                else {
                    if (/*distance_at(pos) == distance &&*/ !deleted_at(pos) &&
                        equal_(value, value_at(pos))) {
                        return pos;
                    }
                }

                ++distance;
                ++pos;
            }
            pos = 0;
        }
    }

    bool vacant_at(std::size_t pos) const noexcept {
        return *reinterpret_cast<const std::uint64_t*>(&raw_storage_.metadata_at(pos)) ==
               0;
    }

public:
    robin_hood_storage() = default;
    robin_hood_storage(const robin_hood_storage& other)
        : robin_hood_storage(other.begin(), other.end()) {}
    robin_hood_storage(robin_hood_storage&&) = default;

    explicit robin_hood_storage(std::size_t capacity,
                                const Allocator& alloc = Allocator{})
        : raw_storage_(capacity, alloc) {}

    template <
        typename InputIter,
        typename std::enable_if_t<
            std::is_same<typename std::iterator_traits<InputIter>::iterator_category,
                         std::input_iterator_tag>::value,
            int> = 0>
    robin_hood_storage(InputIter first, InputIter last, const Hash& hash,
                       const Equal& equal, const Allocator& alloc)
        : raw_storage_(alloc), hash_(hash), equal_(equal) {
        for (; first != last; ++first) {
            insert(*first);
        }
    }

    template <
        typename InputIter,
        typename std::enable_if_t<std::is_convertible<typename std::iterator_traits<
                                                          InputIter>::iterator_category,
                                                      std::forward_iterator_tag>::value,
                                  unsigned> = 0>
    robin_hood_storage(InputIter first, InputIter last, const Hash& hash,
                       const Equal& equal, const Allocator& alloc)
        : raw_storage_(0, alloc), hash_(hash), equal_(equal) {
        std::size_t capacity;
        std::tie(capacity, to_position_) =
            growth_policy::get((std::distance(first, last) * 10) / 8 + 1);
        raw_storage_ = raw_storage_type{capacity, alloc};
        for (; first != last; ++first) {
            insert_impl(*first);
        }
    }

    robin_hood_storage& operator=(const robin_hood_storage& other) {
        robin_hood_storage other_copy{other.begin(), other.end(), hash_, equal_,
                                      raw_storage_.allocator()};
        *this = std::move(other_copy);
        return *this;
    }
    robin_hood_storage& operator=(robin_hood_storage&&) = default;

    iterator begin() const noexcept {
        std::size_t pos = 0;
        for (; pos < capacity(); ++pos) {
            if (!vacant_at(pos) && !deleted_at(pos)) {
                break;
            }
        }
        return iterator_for(pos);
    }

    iterator end() const noexcept { return iterator_for(capacity()); }

    std::size_t capacity() const noexcept { return raw_storage_.capacity(); }

    bool empty() const noexcept { return size_ == 0; }

    iterator erase(iterator it) {
        STUFF_ASSERT(it.parent_ == this);
        STUFF_ASSERT(it.pos_ <= capacity());
        STUFF_ASSERT(!vacant_at(it.pos_) && !deleted_at(it.pos_));

        auto result = std::next(it);
        erase_at(it.pos_);
        return result;
    }

    void quick_erase(iterator it) {
        STUFF_ASSERT(it.parent_ == this);
        STUFF_ASSERT(it.pos_ <= capacity());
        STUFF_ASSERT(!vacant_at(it.pos_) && !deleted_at(it.pos_));

        erase_at(it.pos_);
    }

    iterator erase(iterator first, iterator last) {
        STUFF_ASSERT(first.parent_ == this);
        STUFF_ASSERT(last.parent_ == this);
        STUFF_ASSERT(first.pos_ <= last.pos_);
        STUFF_ASSERT(last.pos_ <= capacity());
        STUFF_ASSERT(!vacant_at(first.pos_) && !deleted_at(first.pos_));
        STUFF_ASSERT(last == end() || (!vacant_at(last.pos_) && !deleted_at(last.pos_)));

        for (; first != last; ++first) {
            erase_at(first.pos_);
        }
        return last;
    }

    template <typename V>
    std::size_t erase(const V& value) {
        const auto pos = search(value);
        if (pos != capacity()) {
            erase_at(pos);
            return 1;
        }
        return 0;
    }

    std::size_t size() const noexcept { return size_; }

    std::size_t max_size() const noexcept { return max_size_; }

    void clear() noexcept {
        for (std::size_t pos = 0; pos < capacity(); ++pos) {
            if (vacant_at(pos)) {
                continue;
            }
            if (!deleted_at(pos)) {
                raw_storage_.destroy(pos);
            }
            raw_storage_.metadata_at(pos) = metadata::vacant();
        }
        size_ = 0;
    }

    const T& value_at(std::size_t pos) const noexcept {
        return raw_storage_.value_at(pos);
    }

    std::pair<iterator, bool> insert(T value) { return emplace(std::move(value)); }

    template <typename InputIter>
    void insert(InputIter first, InputIter last) {
        for (; first != last; ++first) {
            insert(*first);
        }
    }

    template <typename... Args>
    std::pair<iterator, bool> emplace(Args&&... args) {
        if (10 * size() >= 8 * capacity()) {
            grow();
        }
        return insert_impl(T{std::forward<Args>(args)...});
    }

    template <typename V>
    std::size_t count(const V& value) const noexcept(noexcept(search(value))) {
        return search(value) == capacity() ? 0 : 1;
    }

    template <typename V>
    iterator find(const V& value) const noexcept(noexcept(search(value))) {
        return iterator_for(search(value));
    }
};

}  // namespace stuff::container::detail
