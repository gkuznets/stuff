#pragma once

#include <cstddef>
#include <type_traits>

#include <stuff/meta/math.hpp>

namespace stuff::meta {

struct empty_range {};

template <std::size_t begin_, std::size_t end_>
struct range {
    static const std::size_t begin = begin_;
    static const std::size_t end = end_;
};

template <typename Range>
struct is_empty {};

template <>
struct is_empty<empty_range> {
    constexpr static bool value = true;
};

template <std::size_t begin, std::size_t end>
struct is_empty<range<begin, end>> {
    constexpr static bool value = end <= begin;
};

template <typename Range>
constexpr bool is_empty_v = is_empty<Range>::value;

template <typename Lhs, typename Rhs>
struct difference {};

template <std::size_t begin_lhs, std::size_t end_lhs, std::size_t begin_rhs,
          std::size_t end_rhs>
struct difference<range<begin_lhs, end_lhs>, range<begin_rhs, end_rhs>> {
    using type = std::conditional_t<
        (begin_lhs >= begin_rhs && end_lhs <= end_rhs), empty_range,
        std::conditional_t<(begin_lhs < begin_rhs), range<begin_lhs, begin_rhs>,
                           range<end_rhs, end_lhs>>>;
};

template <typename Lhs, typename Rhs>
using difference_t = typename difference<Lhs, Rhs>::type;

template <typename Lhs, typename Rhs>
struct intersection {};

template <std::size_t begin_lhs, std::size_t end_lhs, std::size_t begin_rhs,
          std::size_t end_rhs>
struct intersection<range<begin_lhs, end_lhs>, range<begin_rhs, end_rhs>> {
    using type =
        std::conditional_t<(end_lhs <= begin_rhs || end_rhs <= begin_lhs), empty_range,
                           range<max_v<begin_lhs, begin_rhs>, min_v<end_lhs, end_rhs>>>;
};

template <typename Lhs, typename Rhs>
using intersection_t = typename intersection<Lhs, Rhs>::type;

}  // namespace stuff::meta
