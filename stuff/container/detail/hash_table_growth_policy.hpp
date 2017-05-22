#pragma once

#include <algorithm>
#include <cstdint>
#include <utility>

#define LIBDIVIDE_USE_SSE2 1
#define LIBDIVIDE_USE_SSE4_1 1

#include <libdivide/libdivide.h>
#include <stuff/assert.hpp>

namespace stuff::container::detail {

class simple_growth_policy {
public:
    class reducer {
        std::uint64_t denom_ = 0;
        libdivide::divider<std::uint64_t> div_ = libdivide::divider<std::uint64_t>(1);

        friend class simple_growth_policy;
        explicit reducer(std::uint64_t denom) noexcept : denom_(denom), div_(denom) {}

    public:
        reducer() = default;
        reducer(const reducer&) noexcept = default;
        reducer(reducer&&) noexcept = default;

        reducer& operator=(const reducer&) noexcept = default;
        reducer& operator=(reducer&&) noexcept = default;

        std::uint64_t operator()(std::uint64_t n) const {
            return n - denom_ * (n / div_);
        }
    };

    static std::pair<std::size_t, reducer> get(std::size_t expected_size) noexcept {
        STUFF_ASSERT(expected_size != 0);

        return {expected_size, reducer{expected_size}};
    }
};

constexpr uint32_t log2(std::uint64_t n) {
    if (n <= 1) {
        return 0;
    }
    n -= 1;
    uint32_t power = 0;
    while (n > 0) {
        ++power;
        n >>= 1;
    }
    return power;
}

class power_of_two_growth_policy {
public:
    class reducer {
        std::uint64_t mask_ = 0xFFFFFFFFFFFFFFFF;

        friend class power_of_two_growth_policy;
        explicit reducer(std::uint64_t mask) noexcept : mask_(mask) {}

    public:
        reducer() = default;
        reducer(const reducer&) noexcept = default;
        reducer(reducer&&) noexcept = default;

        reducer& operator=(const reducer&) noexcept = default;
        reducer& operator=(reducer&&) noexcept = default;

        std::uint64_t operator()(std::uint64_t n) const { return n & mask_; }
    };

    static std::pair<std::size_t, reducer> get(std::size_t expected_size) noexcept {
        const auto pwr = log2(expected_size);
        return {1 << pwr, reducer{~(0xFFFFFFFFFFFFFFFF << pwr)}};
    }
};

class weird_powers_of_two_growth_policy {
    // clang-format off
    static constexpr std::uint64_t bounds[] = {2, 3, 4, 6, 8, 12, 16, 24, 32, 48, 64, 96, 128, 192,
        256, 384, 512, 768, 1024, 1536, 2048, 3072, 4096, 6144, 8192, 12288, 16384, 24576, 32768,
        49152, 65536, 98304, 131072, 196608, 262144, 393216, 524288, 786432, 1048576, 1572864,
        2097152, 3145728, 4194304, 6291456, 8388608, 12582912, 16777216, 25165824, 33554432,
        50331648, 67108864, 100663296, 134217728, 201326592, 268435456, 402653184, 536870912,
        805306368, 1073741824, 1610612736, 2147483648, 3221225472, 4294967296, 6442450944,
        8589934592, 12884901888, 17179869184, 25769803776, 34359738368, 51539607552, 68719476736,
        103079215104, 137438953472, 206158430208, 274877906944, 412316860416, 549755813888,
        824633720832, 1099511627776, 1649267441664, 2199023255552, 3298534883328, 4398046511104,
        6597069766656, 8796093022208, 13194139533312, 17592186044416, 26388279066624,
        35184372088832, 52776558133248, 70368744177664, 105553116266496, 140737488355328,
        211106232532992, 281474976710656, 422212465065984, 562949953421312, 844424930131968,
        1125899906842624, 1688849860263936, 2251799813685248, 3377699720527872, 4503599627370496,
        6755399441055744, 9007199254740992, 13510798882111488, 18014398509481984,
        27021597764222976, 36028797018963968, 54043195528445952, 72057594037927936,
        108086391056891904, 144115188075855872, 216172782113783808, 288230376151711744,
        432345564227567616, 576460752303423488, 864691128455135232, 1152921504606846976,
        1729382256910270464, 2305843009213693952, 3458764513820540928, 4611686018427387904,
        6917529027641081856};

    static std::uint64_t mod_2(std::uint64_t n) { return n % 2; }
    static std::uint64_t mod_3(std::uint64_t n) { return n % 3; }
    static std::uint64_t mod_4(std::uint64_t n) { return n % 4; }
    static std::uint64_t mod_6(std::uint64_t n) { return n % 6; }
    static std::uint64_t mod_8(std::uint64_t n) { return n % 8; }
    static std::uint64_t mod_12(std::uint64_t n) { return n % 12; }
    static std::uint64_t mod_16(std::uint64_t n) { return n % 16; }
    static std::uint64_t mod_24(std::uint64_t n) { return n % 24; }
    static std::uint64_t mod_32(std::uint64_t n) { return n % 32; }
    static std::uint64_t mod_48(std::uint64_t n) { return n % 48; }
    static std::uint64_t mod_64(std::uint64_t n) { return n % 64; }
    static std::uint64_t mod_96(std::uint64_t n) { return n % 96; }
    static std::uint64_t mod_128(std::uint64_t n) { return n % 128; }
    static std::uint64_t mod_192(std::uint64_t n) { return n % 192; }
    static std::uint64_t mod_256(std::uint64_t n) { return n % 256; }
    static std::uint64_t mod_384(std::uint64_t n) { return n % 384; }
    static std::uint64_t mod_512(std::uint64_t n) { return n % 512; }
    static std::uint64_t mod_768(std::uint64_t n) { return n % 768; }
    static std::uint64_t mod_1024(std::uint64_t n) { return n % 1024; }
    static std::uint64_t mod_1536(std::uint64_t n) { return n % 1536; }
    static std::uint64_t mod_2048(std::uint64_t n) { return n % 2048; }
    static std::uint64_t mod_3072(std::uint64_t n) { return n % 3072; }
    static std::uint64_t mod_4096(std::uint64_t n) { return n % 4096; }
    static std::uint64_t mod_6144(std::uint64_t n) { return n % 6144; }
    static std::uint64_t mod_8192(std::uint64_t n) { return n % 8192; }
    static std::uint64_t mod_12288(std::uint64_t n) { return n % 12288; }
    static std::uint64_t mod_16384(std::uint64_t n) { return n % 16384; }
    static std::uint64_t mod_24576(std::uint64_t n) { return n % 24576; }
    static std::uint64_t mod_32768(std::uint64_t n) { return n % 32768; }
    static std::uint64_t mod_49152(std::uint64_t n) { return n % 49152; }
    static std::uint64_t mod_65536(std::uint64_t n) { return n % 65536; }
    static std::uint64_t mod_98304(std::uint64_t n) { return n % 98304; }
    static std::uint64_t mod_131072(std::uint64_t n) { return n % 131072; }
    static std::uint64_t mod_196608(std::uint64_t n) { return n % 196608; }
    static std::uint64_t mod_262144(std::uint64_t n) { return n % 262144; }
    static std::uint64_t mod_393216(std::uint64_t n) { return n % 393216; }
    static std::uint64_t mod_524288(std::uint64_t n) { return n % 524288; }
    static std::uint64_t mod_786432(std::uint64_t n) { return n % 786432; }
    static std::uint64_t mod_1048576(std::uint64_t n) { return n % 1048576; }
    static std::uint64_t mod_1572864(std::uint64_t n) { return n % 1572864; }
    static std::uint64_t mod_2097152(std::uint64_t n) { return n % 2097152; }
    static std::uint64_t mod_3145728(std::uint64_t n) { return n % 3145728; }
    static std::uint64_t mod_4194304(std::uint64_t n) { return n % 4194304; }
    static std::uint64_t mod_6291456(std::uint64_t n) { return n % 6291456; }
    static std::uint64_t mod_8388608(std::uint64_t n) { return n % 8388608; }
    static std::uint64_t mod_12582912(std::uint64_t n) { return n % 12582912; }
    static std::uint64_t mod_16777216(std::uint64_t n) { return n % 16777216; }
    static std::uint64_t mod_25165824(std::uint64_t n) { return n % 25165824; }
    static std::uint64_t mod_33554432(std::uint64_t n) { return n % 33554432; }
    static std::uint64_t mod_50331648(std::uint64_t n) { return n % 50331648; }
    static std::uint64_t mod_67108864(std::uint64_t n) { return n % 67108864; }
    static std::uint64_t mod_100663296(std::uint64_t n) { return n % 100663296; }
    static std::uint64_t mod_134217728(std::uint64_t n) { return n % 134217728; }
    static std::uint64_t mod_201326592(std::uint64_t n) { return n % 201326592; }
    static std::uint64_t mod_268435456(std::uint64_t n) { return n % 268435456; }
    static std::uint64_t mod_402653184(std::uint64_t n) { return n % 402653184; }
    static std::uint64_t mod_536870912(std::uint64_t n) { return n % 536870912; }
    static std::uint64_t mod_805306368(std::uint64_t n) { return n % 805306368; }
    static std::uint64_t mod_1073741824(std::uint64_t n) { return n % 1073741824; }
    static std::uint64_t mod_1610612736(std::uint64_t n) { return n % 1610612736; }
    static std::uint64_t mod_2147483648(std::uint64_t n) { return n % 2147483648; }
    static std::uint64_t mod_3221225472(std::uint64_t n) { return n % 3221225472; }
    static std::uint64_t mod_4294967296(std::uint64_t n) { return n % 4294967296; }
    static std::uint64_t mod_6442450944(std::uint64_t n) { return n % 6442450944; }
    static std::uint64_t mod_8589934592(std::uint64_t n) { return n % 8589934592; }
    static std::uint64_t mod_12884901888(std::uint64_t n) { return n % 12884901888; }
    static std::uint64_t mod_17179869184(std::uint64_t n) { return n % 17179869184; }
    static std::uint64_t mod_25769803776(std::uint64_t n) { return n % 25769803776; }
    static std::uint64_t mod_34359738368(std::uint64_t n) { return n % 34359738368; }
    static std::uint64_t mod_51539607552(std::uint64_t n) { return n % 51539607552; }
    static std::uint64_t mod_68719476736(std::uint64_t n) { return n % 68719476736; }
    static std::uint64_t mod_103079215104(std::uint64_t n) { return n % 103079215104; }
    static std::uint64_t mod_137438953472(std::uint64_t n) { return n % 137438953472; }
    static std::uint64_t mod_206158430208(std::uint64_t n) { return n % 206158430208; }
    static std::uint64_t mod_274877906944(std::uint64_t n) { return n % 274877906944; }
    static std::uint64_t mod_412316860416(std::uint64_t n) { return n % 412316860416; }
    static std::uint64_t mod_549755813888(std::uint64_t n) { return n % 549755813888; }
    static std::uint64_t mod_824633720832(std::uint64_t n) { return n % 824633720832; }
    static std::uint64_t mod_1099511627776(std::uint64_t n) { return n % 1099511627776; }
    static std::uint64_t mod_1649267441664(std::uint64_t n) { return n % 1649267441664; }
    static std::uint64_t mod_2199023255552(std::uint64_t n) { return n % 2199023255552; }
    static std::uint64_t mod_3298534883328(std::uint64_t n) { return n % 3298534883328; }
    static std::uint64_t mod_4398046511104(std::uint64_t n) { return n % 4398046511104; }
    static std::uint64_t mod_6597069766656(std::uint64_t n) { return n % 6597069766656; }
    static std::uint64_t mod_8796093022208(std::uint64_t n) { return n % 8796093022208; }
    static std::uint64_t mod_13194139533312(std::uint64_t n) { return n % 13194139533312; }
    static std::uint64_t mod_17592186044416(std::uint64_t n) { return n % 17592186044416; }
    static std::uint64_t mod_26388279066624(std::uint64_t n) { return n % 26388279066624; }
    static std::uint64_t mod_35184372088832(std::uint64_t n) { return n % 35184372088832; }
    static std::uint64_t mod_52776558133248(std::uint64_t n) { return n % 52776558133248; }
    static std::uint64_t mod_70368744177664(std::uint64_t n) { return n % 70368744177664; }
    static std::uint64_t mod_105553116266496(std::uint64_t n) { return n % 105553116266496; }
    static std::uint64_t mod_140737488355328(std::uint64_t n) { return n % 140737488355328; }
    static std::uint64_t mod_211106232532992(std::uint64_t n) { return n % 211106232532992; }
    static std::uint64_t mod_281474976710656(std::uint64_t n) { return n % 281474976710656; }
    static std::uint64_t mod_422212465065984(std::uint64_t n) { return n % 422212465065984; }
    static std::uint64_t mod_562949953421312(std::uint64_t n) { return n % 562949953421312; }
    static std::uint64_t mod_844424930131968(std::uint64_t n) { return n % 844424930131968; }
    static std::uint64_t mod_1125899906842624(std::uint64_t n) { return n % 1125899906842624; }
    static std::uint64_t mod_1688849860263936(std::uint64_t n) { return n % 1688849860263936; }
    static std::uint64_t mod_2251799813685248(std::uint64_t n) { return n % 2251799813685248; }
    static std::uint64_t mod_3377699720527872(std::uint64_t n) { return n % 3377699720527872; }
    static std::uint64_t mod_4503599627370496(std::uint64_t n) { return n % 4503599627370496; }
    static std::uint64_t mod_6755399441055744(std::uint64_t n) { return n % 6755399441055744; }
    static std::uint64_t mod_9007199254740992(std::uint64_t n) { return n % 9007199254740992; }
    static std::uint64_t mod_13510798882111488(std::uint64_t n) { return n % 13510798882111488; }
    static std::uint64_t mod_18014398509481984(std::uint64_t n) { return n % 18014398509481984; }
    static std::uint64_t mod_27021597764222976(std::uint64_t n) { return n % 27021597764222976; }
    static std::uint64_t mod_36028797018963968(std::uint64_t n) { return n % 36028797018963968; }
    static std::uint64_t mod_54043195528445952(std::uint64_t n) { return n % 54043195528445952; }
    static std::uint64_t mod_72057594037927936(std::uint64_t n) { return n % 72057594037927936; }
    static std::uint64_t mod_108086391056891904(std::uint64_t n) { return n % 108086391056891904; }
    static std::uint64_t mod_144115188075855872(std::uint64_t n) { return n % 144115188075855872; }
    static std::uint64_t mod_216172782113783808(std::uint64_t n) { return n % 216172782113783808; }
    static std::uint64_t mod_288230376151711744(std::uint64_t n) { return n % 288230376151711744; }
    static std::uint64_t mod_432345564227567616(std::uint64_t n) { return n % 432345564227567616; }
    static std::uint64_t mod_576460752303423488(std::uint64_t n) { return n % 576460752303423488; }
    static std::uint64_t mod_864691128455135232(std::uint64_t n) { return n % 864691128455135232; }
    static std::uint64_t mod_1152921504606846976(std::uint64_t n) { return n % 1152921504606846976; }
    static std::uint64_t mod_1729382256910270464(std::uint64_t n) { return n % 1729382256910270464; }
    static std::uint64_t mod_2305843009213693952(std::uint64_t n) { return n % 2305843009213693952; }
    static std::uint64_t mod_3458764513820540928(std::uint64_t n) { return n % 3458764513820540928; }
    static std::uint64_t mod_4611686018427387904(std::uint64_t n) { return n % 4611686018427387904; }
    static std::uint64_t mod_6917529027641081856(std::uint64_t n) { return n % 6917529027641081856; }

    static constexpr std::uint64_t (*mod_fns_[])(std::uint64_t) = { mod_2, mod_3, mod_4, mod_6,
        mod_8, mod_12, mod_16, mod_24, mod_32, mod_48, mod_64, mod_96, mod_128, mod_192, mod_256,
        mod_384, mod_512, mod_768, mod_1024, mod_1536, mod_2048, mod_3072, mod_4096, mod_6144,
        mod_8192, mod_12288, mod_16384, mod_24576, mod_32768, mod_49152, mod_65536, mod_98304,
        mod_131072, mod_196608, mod_262144, mod_393216, mod_524288, mod_786432, mod_1048576,
        mod_1572864, mod_2097152, mod_3145728, mod_4194304, mod_6291456, mod_8388608,
        mod_12582912, mod_16777216, mod_25165824, mod_33554432, mod_50331648, mod_67108864,
        mod_100663296, mod_134217728, mod_201326592, mod_268435456, mod_402653184, mod_536870912,
        mod_805306368, mod_1073741824, mod_1610612736, mod_2147483648, mod_3221225472,
        mod_4294967296, mod_6442450944, mod_8589934592, mod_12884901888, mod_17179869184,
        mod_25769803776, mod_34359738368, mod_51539607552, mod_68719476736, mod_103079215104,
        mod_137438953472, mod_206158430208, mod_274877906944, mod_412316860416, mod_549755813888,
        mod_824633720832, mod_1099511627776, mod_1649267441664, mod_2199023255552,
        mod_3298534883328, mod_4398046511104, mod_6597069766656, mod_8796093022208,
        mod_13194139533312, mod_17592186044416, mod_26388279066624, mod_35184372088832,
        mod_52776558133248, mod_70368744177664, mod_105553116266496, mod_140737488355328,
        mod_211106232532992, mod_281474976710656, mod_422212465065984, mod_562949953421312,
        mod_844424930131968, mod_1125899906842624, mod_1688849860263936, mod_2251799813685248,
        mod_3377699720527872, mod_4503599627370496, mod_6755399441055744, mod_9007199254740992,
        mod_13510798882111488, mod_18014398509481984, mod_27021597764222976, mod_36028797018963968,
        mod_54043195528445952, mod_72057594037927936, mod_108086391056891904,
        mod_144115188075855872, mod_216172782113783808, mod_288230376151711744,
        mod_432345564227567616, mod_576460752303423488, mod_864691128455135232,
        mod_1152921504606846976, mod_1729382256910270464, mod_2305843009213693952,
        mod_3458764513820540928, mod_4611686018427387904, mod_6917529027641081856};
    // clang-format on

public:
    using reducer = std::uint64_t (*)(std::uint64_t);

    static std::pair<std::size_t, reducer> get(std::size_t expected_size) noexcept {
        const auto bound =
            std::lower_bound(std::begin(bounds), std::end(bounds), expected_size);

        return {*bound, mod_fns_[bound - std::begin(bounds)]};
    }
};

}  // namespace stuff::container::detail
