#pragma once

#include <cstddef>

namespace stuff::meta {

template <std::size_t lhs, std::size_t rhs>
struct min {
    static constexpr std::size_t value = lhs <= rhs ? lhs : rhs;
};

template <std::size_t lhs, std::size_t rhs>
constexpr std::size_t min_v = min<lhs, rhs>::value;

template <std::size_t lhs, std::size_t rhs>
struct max {
    static constexpr std::size_t value = lhs >= rhs ? lhs : rhs;
};

template <std::size_t lhs, std::size_t rhs>
constexpr std::size_t max_v = max<lhs, rhs>::value;

}  // namespace stuff::meta
