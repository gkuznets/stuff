#define BOOST_TEST_MODULE meta__named_template_params__test

#include <type_traits>

#include <boost/test/included/unit_test.hpp>
#include <stuff/meta/nth.hpp>

BOOST_AUTO_TEST_CASE(nth) {
    BOOST_CHECK(bool(std::is_same<stuff::meta::nth_t<0, double, float, bool>, double>::value));
    BOOST_CHECK(bool(std::is_same<stuff::meta::nth_t<1, double, float, bool>, float>::value));
    BOOST_CHECK(bool(std::is_same<stuff::meta::nth_t<2, double, float, bool>, bool>::value));
}
