#pragma once

#include <stuff/meta/nth.hpp>

namespace stuff::meta {

namespace detail {

template <typename Head, typename... Tail>
struct head {
    using type = Head;
};

template <typename... Types>
using head_t = typename head<Types...>::type;

}  // namespace detail

template <typename... Types>
struct list {
    static constexpr unsigned size() { return sizeof...(Types); }

    using head = detail::head_t<Types...>;

    template <std::size_t n>
    using nth = nth_t<n, Types...>;
};

}  // namespace stuff::meta
