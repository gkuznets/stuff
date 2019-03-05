#define BOOST_TEST_MODULE meta__named_template_params__test

#include <type_traits>

#include <boost/test/included/unit_test.hpp>
#include <stuff/meta/range.hpp>

BOOST_AUTO_TEST_CASE(difference) {
    BOOST_CHECK(
        (stuff::meta::is_empty_v<stuff::meta::difference_t<stuff::meta::range<1, 2>,
                                                           stuff::meta::range<1, 2>>>));

    BOOST_CHECK(
        (std::is_same_v<
            stuff::meta::difference_t<stuff::meta::range<1, 3>, stuff::meta::range<1, 2>>,
            stuff::meta::range<2, 3>>));

    BOOST_CHECK(
        (std::is_same_v<
            stuff::meta::difference_t<stuff::meta::range<0, 2>, stuff::meta::range<1, 2>>,
            stuff::meta::range<0, 1>>));
}

BOOST_AUTO_TEST_CASE(intersection) {
    BOOST_CHECK(
        (stuff::meta::is_empty_v<stuff::meta::intersection_t<stuff::meta::range<1, 2>,
                                                             stuff::meta::range<2, 3>>>));

    BOOST_CHECK(
        (stuff::meta::is_empty_v<stuff::meta::intersection_t<stuff::meta::range<2, 3>,
                                                             stuff::meta::range<1, 2>>>));

    BOOST_CHECK((std::is_same_v<stuff::meta::intersection_t<stuff::meta::range<0, 3>,
                                                            stuff::meta::range<1, 2>>,
                                stuff::meta::range<1, 2>>));

    BOOST_CHECK((std::is_same_v<stuff::meta::intersection_t<stuff::meta::range<0, 3>,
                                                            stuff::meta::range<1, 4>>,
                                stuff::meta::range<1, 3>>));
}
