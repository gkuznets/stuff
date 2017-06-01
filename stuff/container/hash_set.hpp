#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

#include <stuff/container/detail/robin_hood_storage.hpp>
#include <stuff/container/selectors.hpp>
#include <stuff/meta/named_template_params.hpp>

namespace stuff::container {

namespace detail {

template <typename Key, typename Hash = std::hash<Key>,
          typename Equal = std::equal_to<Key>, typename Allocator = std::allocator<Key>,
          typename... Params>
class hash_set {
    using storage = detail::robin_hood_storage<Key, Key, Hash, Equal, Allocator, Params...>;
    storage storage_;

public:
    using key_type = Key;
    using value_type = Key;
    using size_type = std::size_t;
    using hasher = Hash;
    using key_equal = Equal;
    using allocator_type = Allocator;
    using reference = value_type&;
    using const_reference = const value_type&;
    using iterator = typename storage::const_iterator;
    using const_iterator = iterator;

    hash_set() = default;
    hash_set(const hash_set&) = default;
    hash_set(hash_set&&) = default;

    explicit hash_set(size_type capacity) : storage_(capacity) {}

    template <typename InputIter>
    hash_set(InputIter first, InputIter last, const Hash& hash = Hash{},
             const Equal& equal = Equal{}, const Allocator& alloc = Allocator{})
        : storage_(first, last, hash, equal, alloc) {}

    template <typename InputIter>
    hash_set(InputIter first, InputIter last, const Allocator& alloc)
        : storage_(first, last, Hash{}, Equal{}, alloc) {}

    hash_set(std::initializer_list<value_type> values, const Hash& hash = Hash{},
             const Equal& equal = Equal{}, const Allocator& alloc = Allocator{})
        : storage_(values.begin(), values.end(), hash, equal, alloc) {}

    hash_set& operator=(const hash_set&) = default;
    hash_set& operator=(hash_set&&) = default;

    iterator begin() noexcept { return storage_.begin(); }
    const_iterator begin() const noexcept { return storage_.begin(); }

    iterator end() noexcept { return storage_.end(); }
    const_iterator end() const noexcept { return storage_.end(); }

    //! \return true if the container is empty, false otherwise.
    bool empty() const noexcept { return storage_.empty(); }

    size_type size() const noexcept { return storage_.size(); }

    //! \return Maximum number of elements the container is able to hold.
    size_type max_size() const noexcept { return storage_.max_size(); }

    //! Removes all elements from the container.
    void clear() noexcept { storage_.clear(); }

    std::pair<iterator, bool> insert(const value_type& value) {
        return storage_.insert(value);
    }

    std::pair<iterator, bool> insert(value_type&& value) {
        return storage_.insert(value);
    }

    template <typename InputIter>
    void insert(InputIter first, InputIter last) {
        storage_.insert(first, last);
    }

    void insert(std::initializer_list<value_type> values) {
        storage_.insert(values.begin(), values.end());
    }

    //! Removes the element pointed to by pos.
    //!
    //! References and iterators to the erased element are invalidated.
    //! Other iterators and references are not invalidated.
    //!
    //! \param pos iterator to the element that should be removed from the set.
    //! \return Iterator following the removed element.
    iterator erase(const_iterator pos) { return storage_.erase(pos); }

    //! Removes the element pointed to by pos.
    //!
    //! \param pos iterator to the element that should be removed from the set.
    iterator quick_erase(const_iterator pos) { storage_.quick_erase(pos); }

    //! Removes the elements in the range from first to last.
    iterator erase(const_iterator first, const_iterator last) {
        return storage_.erase(first, last);
    }

    //! \return Number of removed elements (1 or 0).
    size_type erase(const value_type& value) { return storage_.erase(value); }

    //! Returns the number of elements with key that compares equal to the specified
    //! argument key, which is either 1 or 0 since this container does not allow
    //! duplicates.
    size_type count(const value_type& value) const
        noexcept(noexcept(storage_.count(value))) {
        return storage_.count(value);
    }

    const_iterator find(const value_type& key) const
        noexcept(noexcept(storage_.find(key))) {
        return storage_.find(key);
    }

    double load_factor() const noexcept {
        return static_cast<double>(size()) / static_cast<double>(storage_.capacity());
    }
};

template <typename... Ts>
bool operator==(const hash_set<Ts...>& l, const hash_set<Ts...>& r) {
    if (l.size() != r.size()) {
        return false;
    }
    for (const auto& x : l) {
        if (r.count(x) == 0) {
            return false;
        }
    }
    return true;
}

template <typename... Ts>
bool operator!=(const hash_set<Ts...>& l, const hash_set<Ts...>& r) {
    return !(l == r);
}

}  // namespace detail

template <typename Key, typename Hash = std::hash<Key>,
          typename Equal = std::equal_to<Key>, typename Allocator = std::allocator<Key>>
using hash_set = detail::hash_set<Key, Hash, Equal, Allocator>;

template <typename... Params>
using custom_hash_set = detail::hash_set<
    /* Key */ meta::get_t<key<int>, Params...>,
    /* Hash */
    meta::get_t<hash<std::hash<meta::get_t<key<int>, Params...>>>, Params...>,
    /* Equal */
    meta::get_t<equal_to<std::equal_to<meta::get_t<key<int>, Params...>>>, Params...>,
    /* Allocator */
    meta::get_t<allocator<std::allocator<meta::get_t<key<int>, Params...>>>, Params...>,
    Params...>;

}  // namespace stuff::container
