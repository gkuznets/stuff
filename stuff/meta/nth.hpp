#pragma once

#include <cstddef>
#include <type_traits>

namespace stuff::meta {

template <std::size_t n, typename... Ts>
struct nth;

template <std::size_t n>
struct nth<n> {
    using type = void;
};

template <std::size_t n, typename T, typename... Ts>
struct nth<n, T, Ts...> {
    using type = std::conditional_t<n == 0, T, typename nth<n - 1, Ts...>::type>;
};

template <std::size_t n, typename... Ts>
using nth_t = typename nth<n, Ts...>::type;

}  // namespace stuff::meta
