#define BOOST_TEST_MODULE container__adaptive_radix_tree__test

#include <boost/test/included/unit_test.hpp>
#include <stuff/container/adaptive_radix_tree.hpp>

namespace sc = stuff::container;

BOOST_AUTO_TEST_CASE(art_of_int) {
    sc::adaptive_radix_tree<int> art;
    BOOST_CHECK(art.empty());
}

BOOST_AUTO_TEST_CASE(art_of_int__insert) {
    sc::adaptive_radix_tree<int> art;
    art.insert(1, 2);
}

BOOST_AUTO_TEST_CASE(art_of_int__find_in_empty_tree) {
    sc::adaptive_radix_tree<int> art;
    BOOST_CHECK_EQUAL(art.find(12345), nullptr);
}

BOOST_AUTO_TEST_CASE(art_of_int__find_in_size_1) {
    sc::adaptive_radix_tree<int> art;
    art.insert(1, 10);
    const auto result = art.find(1);
    BOOST_CHECK(result);
    BOOST_CHECK_EQUAL(*result, 10);
    BOOST_CHECK_EQUAL(art.find(12345), nullptr);
}

BOOST_AUTO_TEST_CASE(art_of_int__find_in_size_5) {
    sc::adaptive_radix_tree<int> art;
    art.insert(1, 10);
    art.insert(10, 100);
    art.insert(100, 1000);
    art.insert(1000, 10000);
    art.insert(10000, 100000);
    for (int i = 1; i <= 10000; i *= 10) {
        const auto result = art.find(i);
        BOOST_CHECK(result);
        BOOST_CHECK_EQUAL(*result, i * 10);
    }
    BOOST_CHECK_EQUAL(art.find(12345), nullptr);
}

BOOST_AUTO_TEST_CASE(art_of_int__find_in_size_17) {
    sc::adaptive_radix_tree<int> art;
    for (int i = 1; i < 1 << 17; i *= 2) {
        art.insert(i, i * 10);
    }
    for (int i = 1; i < 1 << 17; i *= 2) {
        const auto result = art.find(i);
        BOOST_CHECK(result);
        BOOST_CHECK_EQUAL(*result, i * 10);
    }
    BOOST_CHECK_EQUAL(art.find(12345), nullptr);
}

BOOST_AUTO_TEST_CASE(art_of_int__find_in_size_10000) {
    const int sz = 1000;
    sc::adaptive_radix_tree<int> art;
    for (int i = 1; i < sz; ++i) {
        art.insert(i, i * 10);
    }
    for (int i = 1; i < sz; ++i) {
        const auto result = art.find(i);
        BOOST_CHECK(result);
        BOOST_CHECK_EQUAL(*result, i * 10);
    }
    BOOST_CHECK_EQUAL(art.find(12345), nullptr);
    //art.dump();
}

BOOST_AUTO_TEST_CASE(art_of_int__erase) {
    sc::adaptive_radix_tree<int> art;
    art.insert(1, 10);
    art.insert(10, 100);
    art.insert(100, 1000);
    art.insert(1000, 10000);
    art.insert(10000, 100000);

    BOOST_CHECK(art.erase(1));
    BOOST_CHECK(art.erase(10));
    BOOST_CHECK(art.erase(100));
    BOOST_CHECK(art.erase(1000));
    BOOST_CHECK(art.erase(10000));
    BOOST_CHECK(art.empty());
}
