#define BOOST_TEST_MODULE meta__named_template_params__test

#include <type_traits>

#include <boost/test/included/unit_test.hpp>
#include <stuff/meta/named_template_params.hpp>

DECLARE_TYPE_SELECTOR(foo_opt);
DECLARE_TYPE_SELECTOR(bar_opt);

DECLARE_VALUE_SELECTOR(bool_opt, bool);
DECLARE_VALUE_SELECTOR(int_opt, int);

template <typename... Params>
struct frob {
    using foo = stuff::meta::get_t<foo_opt<int>, Params...>;
    using bar = stuff::meta::get_t<bar_opt<int>, Params...>;

    static constexpr bool bool_value = stuff::meta::get_v<bool_opt<true>, Params...>;
    static constexpr int int_value = stuff::meta::get_v<int_opt<10>, Params...>;
};

BOOST_AUTO_TEST_CASE(get_t) {
    using frob__default = frob<>;
    BOOST_CHECK(bool(std::is_same<typename frob__default::foo, int>::value));
    BOOST_CHECK(bool(std::is_same<typename frob__default::bar, int>::value));

    using frob__default_with_garbage = frob<int, char, bool>;
    BOOST_CHECK(bool(std::is_same<typename frob__default_with_garbage::foo, int>::value));
    BOOST_CHECK(bool(std::is_same<typename frob__default_with_garbage::bar, int>::value));

    using frob__foo_is_double = frob<foo_opt<double>>;
    BOOST_CHECK(bool(std::is_same<typename frob__foo_is_double::foo, double>::value));
    BOOST_CHECK(bool(std::is_same<typename frob__foo_is_double::bar, int>::value));

    using frob__foo_is_double__bar_is_bool = frob<foo_opt<double>, bar_opt<bool>>;
    BOOST_CHECK(bool(
        std::is_same<typename frob__foo_is_double__bar_is_bool::foo, double>::value));
    BOOST_CHECK(
        bool(std::is_same<typename frob__foo_is_double__bar_is_bool::bar, bool>::value));
}

BOOST_AUTO_TEST_CASE(get_v) {
    using frob__default = frob<>;
    BOOST_CHECK_EQUAL(frob__default::bool_value, true);
    BOOST_CHECK_EQUAL(frob__default::int_value, 10);

    using frob__false_100500 = frob<bool_opt<false>, int_opt<100500>>;
    BOOST_CHECK_EQUAL(frob__false_100500::bool_value, false);
    BOOST_CHECK_EQUAL(frob__false_100500::int_value, 100500);

    using frob__false_100500_with_garbage = frob<int, float, bool_opt<false>, int_opt<100500>, int, float>;
    BOOST_CHECK_EQUAL(frob__false_100500_with_garbage::bool_value, false);
    BOOST_CHECK_EQUAL(frob__false_100500_with_garbage::int_value, 100500);
}
