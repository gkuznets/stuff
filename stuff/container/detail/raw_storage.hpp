#pragma once

#include <cstddef>
#include <cstring>
#include <memory>
#include <type_traits>

#include <stuff/assert.hpp>
#include <stuff/meta/nth.hpp>

namespace stuff::container::detail {

template <typename T>
struct zeroable {};

template <typename T>
struct strip_zeroable {
    using type = T;
};

template <typename T>
struct strip_zeroable<zeroable<T>> {
    using type = T;
};

template <typename T>
using strip_zeroable_t = typename strip_zeroable<T>::type;

template <typename Allocator, typename... Ts>
class arrays;

template <typename Allocator>
class arrays<Allocator> {
public:
    void allocate(std::size_t, const Allocator&) {}
    void deallocate(std::size_t, const Allocator&) {}
};

template <typename Allocator, typename T, typename... Ts>
class arrays<Allocator, T, Ts...> {
    using t_stripped = strip_zeroable_t<T>;
    using t_allocator =
        typename std::allocator_traits<Allocator>::template rebind_alloc<t_stripped>;

    typename std::allocator_traits<t_allocator>::pointer head_;
    arrays<Allocator, Ts...> tail_;

public:
    // Exception safety:
    void allocate(std::size_t capacity, const Allocator& alloc) {
        t_allocator t_alloc{alloc};
        head_ = t_alloc.allocate(capacity);
        if
            constexpr(std::is_same<T, zeroable<t_stripped>>::value) {
                std::memset(static_cast<void*>(head_), 0, capacity * sizeof(t_stripped));
            }

        try {
            tail_.allocate(capacity, alloc);
        } catch (...) {
            throw;
        }
    }

    void deallocate(std::size_t capacity, const Allocator& alloc) {
        t_allocator t_alloc{alloc};
        t_alloc.deallocate(head_, capacity);
        tail_.deallocate(capacity, alloc);
    }

    template <size_t n>
    std::add_lvalue_reference_t<meta::nth_t<n, t_stripped, strip_zeroable_t<Ts>...>> get(
        std::size_t pos) noexcept {
        if
            constexpr(n == 0) { return head_[pos]; }
        else {
            return tail_.template get<n - 1>(pos);
        }
    }

    template <size_t n>
    std::add_const_t<
        std::add_lvalue_reference_t<meta::nth_t<n, t_stripped, strip_zeroable_t<Ts>...>>>
    get(std::size_t pos) const noexcept {
        if
            constexpr(n == 0) { return head_[pos]; }
        else {
            return tail_.template get<n - 1>(pos);
        }
    }
};

template <typename Allocator, typename... Ts>
class raw_storage {
    arrays<Allocator, Ts...> data_;
    std::size_t capacity_ = 0;
    Allocator alloc_;

public:
    explicit raw_storage(const Allocator& alloc = Allocator{}) : alloc_(alloc) {}
    explicit raw_storage(std::size_t capacity, const Allocator& alloc = Allocator{})
        : capacity_(capacity), alloc_(alloc) {
        if (capacity_ > 0) {
            data_.allocate(capacity_, alloc_);
        }
    }
    raw_storage(raw_storage&& other) noexcept {
        swap(data_, other.data_);
        std::swap(capacity_, other.capacity_);
        swap(alloc_, other.alloc_);
    }

    ~raw_storage() {
        if (capacity_ > 0) {
            data_.deallocate(capacity_, alloc_);
        }
    }

    raw_storage& operator=(raw_storage&& other) noexcept {
        if (&other != this) {
            swap(data_, other.data_);
            std::swap(capacity_, other.capacity_);
            swap(alloc_, other.alloc_);
        }
        return *this;
    }

    std::size_t capacity() const noexcept { return capacity_; }

    const Allocator& allocator() const { return alloc_; }

    // clang-format off
    template <std::size_t n>
    std::enable_if_t<
        n < sizeof...(Ts),
        std::add_lvalue_reference_t<meta::nth_t<n, strip_zeroable_t<Ts>...>>>
    get(std::size_t pos) noexcept {
        STUFF_ASSERT(pos < capacity_);

        return data_.template get<n>(pos);
    }

    template <std::size_t n>
    std::enable_if_t<
        n < sizeof...(Ts),
        std::add_const_t<std::add_lvalue_reference_t<meta::nth_t<n, strip_zeroable_t<Ts>...>>>>
    get(std::size_t pos) const noexcept {
        STUFF_ASSERT(pos < capacity_);

        return data_.template get<n>(pos);
    }
    // clang-format on

    template <std::size_t n, typename... Args>
    void construct(std::size_t pos, Args&&... args) {
        STUFF_ASSERT(pos < capacity_);

        using U = meta::nth_t<n, Ts...>;
        new (&data_.template get<n>(pos)) U(std::forward<Args>(args)...);
    }

    template <std::size_t n>
    void destroy(std::size_t pos) {
        STUFF_ASSERT(pos < capacity_);

        using U = meta::nth_t<n, Ts...>;
        data_.template get<n>(pos).~U();
    }
};

}  // namespace stuff::container::detail
