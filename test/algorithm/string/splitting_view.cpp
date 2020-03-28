#define BOOST_TEST_MODULE algorithm__string__splitting_view__test

#include <boost/test/included/unit_test.hpp>

#include <stuff/algorithm/string/splitting_view.hpp>

template <typename R>
constexpr std::size_t size(const R& range) noexcept {
    std::size_t result = 0;
    for (auto it = range.begin(); it != range.end(); ++it) {
        ++result;
    }
    return result;
}

BOOST_AUTO_TEST_CASE(emty_string_view__no_separator__merging) {
    BOOST_CHECK_EQUAL(size(stuff::split({}, ' ')), 0);
}

BOOST_AUTO_TEST_CASE(uncompressed_edge_cases__no_separator_merging) {
    BOOST_CHECK_EQUAL(size(stuff::split(" ", ' ')), 2);
    BOOST_CHECK_EQUAL(size(stuff::split("  ", ' ')), 3);
}

BOOST_AUTO_TEST_CASE(uncompressed_general_case__no_separator_merging) {
    BOOST_CHECK_EQUAL(size(stuff::split("1 2 3 4", ' ')), 4);
    BOOST_CHECK_EQUAL(size(stuff::split("1 2  3   4", ' ')), 7);
}

BOOST_AUTO_TEST_CASE(emty_string_view__with_separator__merging) {
    BOOST_CHECK_EQUAL(size(stuff::split({}, ' ', stuff::merge_adjacent_separators::yes)), 0);
}

BOOST_AUTO_TEST_CASE(uncompressed_edge_cases__with_separator_merging) {
    BOOST_CHECK_EQUAL(size(stuff::split(" ", ' ', stuff::merge_adjacent_separators::yes)), 2);
    BOOST_CHECK_EQUAL(size(stuff::split("  ", ' ', stuff::merge_adjacent_separators::yes)), 2);
}

BOOST_AUTO_TEST_CASE(uncompressed_general_case__with_separator_merging) {
    BOOST_CHECK_EQUAL(size(stuff::split("1 2 3 4", ' ')), 4);
    BOOST_CHECK_EQUAL(size(stuff::split("1 2  3   4", ' ', stuff::merge_adjacent_separators::yes)), 4);
}
