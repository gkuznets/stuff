#pragma once

#include <cstdint>

namespace stuff {

namespace detail {

template <unsigned width>
struct uint_t;

template <>
struct uint_t<8> {
    using type = std::uint8_t;
};

template <>
struct uint_t<16> {
    using type = std::uint16_t;
};

template <>
struct uint_t<32> {
    using type = std::uint32_t;
};

template <>
struct uint_t<64> {
    using type = std::uint64_t;
};

}  // namespace detail

template <unsigned width>
using uint_t = typename detail::uint_t<width>::type;

}  // namespace stuff
