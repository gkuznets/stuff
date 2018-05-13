#pragma once

namespace stuff::util::test {

template <unsigned a>
struct alignas(a) alignment {};

class constructions_counter {
    static int counter_;

public:
    static void reset() noexcept { counter_ = 0; };
    static int value() noexcept { return counter_; };

    constructions_counter() { ++counter_; }
    constructions_counter(const constructions_counter&) { ++counter_; }
    constructions_counter(constructions_counter&&) { ++counter_; }
};

#define DECLARE_CONSTRUCTIONS_COUNTER \
    int stuff::util::test::constructions_counter::counter_ = 0;

class destructions_counter {
    static int counter_;

public:
    static void reset() noexcept { counter_ = 0; };
    static int value() noexcept { return counter_; };

    ~destructions_counter() { ++counter_; }
};

#define DECLARE_DESTRUCTIONS_COUNTER \
    int stuff::util::test::destructions_counter::counter_ = 0;

template <typename... Policy>
class type : public Policy... {};

}  // namespace stuff::util::test
