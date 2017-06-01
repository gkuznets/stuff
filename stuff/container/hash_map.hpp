#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <stdexcept>
#include <utility>

#include <stuff/container/detail/robin_hood_storage.hpp>
#include <stuff/container/selectors.hpp>
#include <stuff/meta/named_template_params.hpp>

namespace stuff::container {

namespace detail {

template <typename Key, typename Value, typename Hash = std::hash<Key>,
          typename Equal = std::equal_to<Key>,
          typename Allocator = std::allocator<std::pair<Key, Value>>, typename... Params>
class hash_map {
    struct key_value_hash {
        Hash hash;

        std::size_t operator()(const std::pair<Key, Value>& kv) const
            noexcept(noexcept(hash(kv.first))) {
            return hash(kv.first);
        }

        std::size_t operator()(const Key& key) const noexcept(noexcept(hash(key))) {
            return hash(key);
        }
    };

    struct key_value_equal {
        Equal equal;

        bool operator()(const std::pair<Key, Value>& l,
                        const std::pair<Key, Value>& r) const
            noexcept(noexcept(equal(l.first, r.first))) {
            return equal(l.first, r.first);
        }

        bool operator()(const Key& l, const std::pair<Key, Value>& r) const
            noexcept(noexcept(equal(l, r.first))) {
            return equal(l, r.first);
        }
    };

    using storage = detail::robin_hood_storage<Key, std::pair<Key, Value>, key_value_hash,
                                               key_value_equal, Allocator, Params...>;
    storage storage_;

public:
    using key_type = Key;
    using mapped_type = Value;
    using value_type = std::pair<const Key, Value>;
    using size_type = std::size_t;
    using hasher = Hash;
    using key_equal = Equal;
    using allocator_type = Allocator;
    using reference = value_type&;
    using const_reference = const value_type&;
    using iterator = typename storage::iterator;
    using const_iterator = typename storage::const_iterator;

    hash_map() = default;
    hash_map(const hash_map&) = default;
    hash_map(hash_map&&) = default;

    template <typename InputIter>
    hash_map(InputIter first, InputIter last, const Hash& hash = Hash{},
             const Equal& equal = Equal{}, const Allocator& alloc = Allocator{})
        : storage_(first, last, key_value_hash{hash}, key_value_equal{equal}, alloc) {}

    template <typename InputIter>
    hash_map(InputIter first, InputIter last, const Allocator& alloc)
        : storage_(first, last, key_value_hash{Hash{}}, key_value_equal{Equal{}}, alloc) {
    }

    hash_map(std::initializer_list<value_type> values, const Hash& hash = Hash{},
             const Equal& equal = Equal{}, const Allocator& alloc = Allocator{})
        : storage_(values.begin(), values.end(), key_value_hash{hash},
                   key_value_equal{equal}, alloc) {}

    hash_map& operator=(const hash_map&) = default;
    hash_map& operator=(hash_map&&) = default;

    iterator begin() noexcept { return storage_.begin(); }
    const_iterator begin() const noexcept { return storage_.begin(); }

    iterator end() noexcept { return storage_.end(); }
    const_iterator end() const noexcept { return storage_.end(); }

    bool empty() const noexcept { return storage_.empty(); }

    size_type size() const noexcept { return storage_.size(); }

    void clear() noexcept { storage_.clear(); }

    std::pair<iterator, bool> insert(const value_type& value) {
        return storage_.insert(value);
    }

    template <typename InputIter>
    void insert(InputIter first, InputIter last) {
        storage_.insert(first, last);
    }

    void insert(std::initializer_list<value_type> values) {
        storage_.insert(values.begin(), values.end());
    }

    template <typename V>
    std::pair<iterator, bool> insert_or_assign(const key_type& key, V&& value) {
        auto ret = insert(value_type{key, std::forward<V>(value)});
        if (!ret.second) {
            ret.first.second = std::forward<V>(value);
        }
        return ret;
    }

    template <class... Args>
    std::pair<iterator, bool> try_emplace(const key_type& key, Args&&... args) {
        return storage_.emplace(key, std::forward<Args>(args)...);
    }

    iterator erase(const_iterator pos) { return storage_.erase(pos); }

    iterator erase(const_iterator first, const_iterator last) {
        return storage_.erase(first, last);
    }

    size_type erase(const key_type& key) { return storage_.erase(key); }

    mapped_type& at(const key_type& key) {
        const auto it = storage_.find(key);
        if (it != end()) {
            return it->second;
        }
        throw std::out_of_range{"key not found"};
    }

    const mapped_type& at(const key_type& key) const {
        const auto it = storage_.find(key);
        if (it != end()) {
            return it->second;
        }
        throw std::out_of_range{"key not found"};
    }

    mapped_type& operator[] (const key_type& key) {
        return try_emplace(key).first->second;
    }

    size_type count(const key_type& key) const noexcept(noexcept(storage_.count(key))) {
        return storage_.count(key);
    }

    const_iterator find(const key_type& key) const
        noexcept(noexcept(storage_.find(key))) {
        return storage_.find(key);
    }

    double load_factor() const noexcept {
        return static_cast<double>(size()) / static_cast<double>(storage_.capacity());
    }

    std::size_t capacity() const noexcept { return storage_.capacity(); }
};

template <typename... Ts>
bool operator==(const hash_map<Ts...>& l, const hash_map<Ts...>& r) {
    if (l.size() != r.size()) {
        return false;
    }
    for (const auto& x : l) {
        const auto it = r.find(x.first);
        if (it == r.end() || it->second != x.first) {
            return false;
        }
    }
    return true;
}

template <typename... Ts>
bool operator!=(const hash_map<Ts...>& l, const hash_map<Ts...>& r) {
    return !(l == r);
}

}  // namespace detail

template <typename Key, typename Value, typename Hash = std::hash<Key>,
          typename Equal = std::equal_to<Key>,
          typename Allocator = std::allocator<std::pair<Key, Value>>>
using hash_map = detail::hash_map<Key, Value, Hash, Equal, Allocator>;

template <typename... Params>
using custom_hash_map = detail::hash_map<
    /* Key */ meta::get_t<key<int>, Params...>,
    /* Value */ meta::get_t<value<int>, Params...>,
    /* Hash */
    meta::get_t<hash<std::hash<meta::get_t<key<int>, Params...>>>, Params...>,
    /* Equal */
    meta::get_t<equal_to<std::equal_to<meta::get_t<key<int>, Params...>>>, Params...>,
    /* Allocator */
    meta::get_t<allocator<std::allocator<std::pair<meta::get_t<key<int>, Params...>,
                                                   meta::get_t<value<int>, Params...>>>>,
                Params...>,
    Params...>;

}  // namespace stuff::container
