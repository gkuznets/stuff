#define BOOST_TEST_MODULE container__hash_map__test

#include <boost/test/included/unit_test.hpp>
#include <stuff/container/hash_map.hpp>

#include <iterator>
#include <string>
#include <vector>

using namespace std::string_literals;

BOOST_AUTO_TEST_CASE(hash_map_of_int_int__operator_copy) {
    stuff::container::hash_map<int, int> m1 = {{1, 10}, {2, 20}, {3, 30}};
    stuff::container::hash_map<int, int> m2 = {{4, 40}, {5, 50}, {6, 60}};
    stuff::container::hash_map<int, int> m3;

    m3 = m1;
    BOOST_CHECK(m3.find(1) != m3.end() && *m3.find(1) == std::make_pair(1, 10));
    BOOST_CHECK(m3.find(2) != m3.end() && *m3.find(2) == std::make_pair(2, 20));
    BOOST_CHECK(m3.find(3) != m3.end() && *m3.find(3) == std::make_pair(3, 30));
    m1.clear();
    BOOST_CHECK(m3.find(1) != m3.end() && *m3.find(1) == std::make_pair(1, 10));
    BOOST_CHECK(m3.find(2) != m3.end() && *m3.find(2) == std::make_pair(2, 20));
    BOOST_CHECK(m3.find(3) != m3.end() && *m3.find(3) == std::make_pair(3, 30));

    m3 = m2;
    BOOST_CHECK(m3.find(4) != m3.end() && *m3.find(4) == std::make_pair(4, 40));
    BOOST_CHECK(m3.find(5) != m3.end() && *m3.find(5) == std::make_pair(5, 50));
    BOOST_CHECK(m3.find(6) != m3.end() && *m3.find(6) == std::make_pair(6, 60));
    m3.clear();
    BOOST_CHECK_EQUAL(m2.count(4), 1);
    BOOST_CHECK_EQUAL(m2.count(5), 1);
    BOOST_CHECK_EQUAL(m2.count(6), 1);
}

BOOST_AUTO_TEST_CASE(hash_map_of_string_int__operator_copy) {
    stuff::container::hash_map<std::string, int> m1 = {
        {"1"s, 10}, {"2"s, 20}, {"3"s, 30}};
    stuff::container::hash_map<std::string, int> m2 = {
        {"4"s, 40}, {"5"s, 50}, {"6"s, 60}};
    stuff::container::hash_map<std::string, int> m3;

    m3 = m1;
    BOOST_CHECK(m3.find("1"s) != m3.end() && *m3.find("1"s) == std::make_pair("1"s, 10));
    BOOST_CHECK(m3.find("2"s) != m3.end() && *m3.find("2"s) == std::make_pair("2"s, 20));
    BOOST_CHECK(m3.find("3"s) != m3.end() && *m3.find("3"s) == std::make_pair("3"s, 30));
    m1.clear();
    BOOST_CHECK(m3.find("1"s) != m3.end() && *m3.find("1"s) == std::make_pair("1"s, 10));
    BOOST_CHECK(m3.find("2"s) != m3.end() && *m3.find("2"s) == std::make_pair("2"s, 20));
    BOOST_CHECK(m3.find("3"s) != m3.end() && *m3.find("3"s) == std::make_pair("3"s, 30));

    m3 = m2;
    BOOST_CHECK(m3.find("4"s) != m3.end() && *m3.find("4"s) == std::make_pair("4"s, 40));
    BOOST_CHECK(m3.find("5"s) != m3.end() && *m3.find("5"s) == std::make_pair("5"s, 50));
    BOOST_CHECK(m3.find("6"s) != m3.end() && *m3.find("6"s) == std::make_pair("6"s, 60));
    m3.clear();
    BOOST_CHECK_EQUAL(m2.count("4"s), 1);
    BOOST_CHECK_EQUAL(m2.count("5"s), 1);
    BOOST_CHECK_EQUAL(m2.count("6"s), 1);
}

BOOST_AUTO_TEST_CASE(hash_map_of_int_int__empty) {
    stuff::container::hash_map<int, int> m;
    BOOST_CHECK(m.empty());
    m.insert({1, 10});
    BOOST_CHECK(!m.empty());
}

BOOST_AUTO_TEST_CASE(hash_map_of_string_int__empty) {
    stuff::container::hash_map<std::string, int> m;
    BOOST_CHECK(m.empty());
    m.insert({"1"s, 10});
    BOOST_CHECK(!m.empty());
}

BOOST_AUTO_TEST_CASE(hash_map_of_int_int__size) {
    stuff::container::hash_map<int, int> m;
    BOOST_CHECK_EQUAL(m.size(), 0);
    m.insert({1, 10});
    BOOST_CHECK_EQUAL(m.size(), 1);
    m.insert({2, 20});
    BOOST_CHECK_EQUAL(m.size(), 2);
}

BOOST_AUTO_TEST_CASE(hash_map_of_string_int__size) {
    stuff::container::hash_map<std::string, int> s;
    BOOST_CHECK_EQUAL(s.size(), 0);
    s.insert({"1"s, 10});
    BOOST_CHECK_EQUAL(s.size(), 1);
    s.insert({"2"s, 10});
    BOOST_CHECK_EQUAL(s.size(), 2);
}

BOOST_AUTO_TEST_CASE(hash_map_of_int_int__clear) {
    stuff::container::hash_map<int, int> s;
    s.clear();
    BOOST_CHECK(s.empty());
    s.insert({1, 10});
    s.insert({2, 20});
    s.insert({3, 30});
    s.insert({4, 40});
    s.clear();
    BOOST_CHECK(s.empty());
}

BOOST_AUTO_TEST_CASE(hash_map_of_string_int__clear) {
    stuff::container::hash_map<std::string, int> s;
    s.clear();
    BOOST_CHECK(s.empty());
    s.insert({"1"s, 10});
    s.insert({"2"s, 20});
    s.insert({"3"s, 30});
    s.insert({"4"s, 40});
    s.clear();
    BOOST_CHECK(s.empty());
}

BOOST_AUTO_TEST_CASE(hash_map_of_int_int__insert) {
    stuff::container::hash_map<int, int> s;
    auto ret = s.insert({1, 10});
    BOOST_CHECK(ret.second);
    BOOST_CHECK_EQUAL(ret.first->first, 1);
    BOOST_CHECK_EQUAL(ret.first->second, 10);

    ret = s.insert({1, 20});
    BOOST_CHECK(!ret.second);
    BOOST_CHECK_EQUAL(ret.first->first, 1);
    BOOST_CHECK_EQUAL(ret.first->second, 10);
}

BOOST_AUTO_TEST_CASE(hash_map_of_string_int__insert) {
    stuff::container::hash_map<std::string, int> s;
    auto ret = s.insert({"1"s, 10});
    BOOST_CHECK(ret.second);
    BOOST_CHECK_EQUAL(ret.first->first, "1"s);
    BOOST_CHECK_EQUAL(ret.first->second, 10);

    ret = s.insert({"1"s, 20});
    BOOST_CHECK(!ret.second);
    BOOST_CHECK_EQUAL(ret.first->first, "1"s);
    BOOST_CHECK_EQUAL(ret.first->second, 10);
}

BOOST_AUTO_TEST_CASE(hash_map_of_int_int__erase_iter) {
    stuff::container::hash_map<int, int> m = {{1, 10}, {2, 20}, {3, 30}};
    auto remaining_elements = m.size();
    for (auto it = m.begin(); it != m.end();) {
        auto value = *it;
        it = m.erase(it);
        BOOST_CHECK_EQUAL(m.count(value.first), 0);
        if (--remaining_elements > 0) {
            BOOST_CHECK(it != m.end());
        } else {
            BOOST_CHECK(it == m.end());
        }
    }
    BOOST_CHECK_EQUAL(remaining_elements, 0);
}

BOOST_AUTO_TEST_CASE(hash_map_of_string_int__erase_iter) {
    stuff::container::hash_map<std::string, int> m = {{"1"s, 10}, {"2"s, 20}, {"3"s, 30}};
    auto remaining_elements = m.size();
    for (auto it = m.begin(); it != m.end();) {
        auto value = *it;
        it = m.erase(it);
        BOOST_CHECK_EQUAL(m.count(value.first), 0);
        if (--remaining_elements > 0) {
            BOOST_CHECK(it != m.end());
        } else {
            BOOST_CHECK(it == m.end());
        }
    }
    BOOST_CHECK_EQUAL(remaining_elements, 0);
}

BOOST_AUTO_TEST_CASE(hash_map_of_int_int__erase_range) {
    stuff::container::hash_map<int, int> m = {
        {1, 10}, {2, 20}, {3, 30}, {4, 40}, {5, 50}};
    std::vector<std::pair<int, int>> to_be_removed{std::next(m.begin()),
                                                   std::next(m.begin(), 4)};
    m.erase(std::next(m.begin()), std::next(m.begin(), 4));
    for (const auto& x : to_be_removed) {
        BOOST_CHECK_EQUAL(m.count(x.first), 0);
    }
}

BOOST_AUTO_TEST_CASE(hash_map_of_string_int__erase_range) {
    stuff::container::hash_map<std::string, int> m = {
        {"1"s, 10}, {"2"s, 20}, {"3"s, 30}, {"4"s, 40}, {"5"s, 50}};
    std::vector<std::pair<std::string, int>> to_be_removed{std::next(m.begin()),
                                                           std::next(m.begin(), 4)};
    m.erase(std::next(m.begin()), std::next(m.begin(), 4));
    for (const auto& x : to_be_removed) {
        BOOST_CHECK_EQUAL(m.count(x.first), 0);
    }
}

BOOST_AUTO_TEST_CASE(hash_map_of_int_int_erase_value) {
    stuff::container::hash_map<int, int> m;
    m.insert({1, 10});
    BOOST_CHECK_EQUAL(m.erase(0), 0);
    BOOST_CHECK_EQUAL(m.erase(1), 1);
    BOOST_CHECK_EQUAL(m.erase(1), 0);
}

BOOST_AUTO_TEST_CASE(hash_map_of_string_int__erase_value) {
    stuff::container::hash_map<std::string, int> m;
    m.insert({"1"s, 10});
    BOOST_CHECK_EQUAL(m.erase("0"s), 0);
    BOOST_CHECK_EQUAL(m.erase("1"s), 1);
    BOOST_CHECK_EQUAL(m.erase("1"s), 0);
}

BOOST_AUTO_TEST_CASE(empty__hash_map_of_int_int__find) {
    stuff::container::hash_map<int, int> m;
    BOOST_CHECK(m.find(1) == m.end());
}

BOOST_AUTO_TEST_CASE(empty__hash_map_of_string_int__find) {
    stuff::container::hash_map<std::string, int> m;
    BOOST_CHECK(m.find("1"s) == m.end());
}

BOOST_AUTO_TEST_CASE(nonempty__hash_map_of_int_int__find) {
    stuff::container::hash_map<int, int> m = {
        {1, 10}, {2, 20}, {3, 30}, {4, 40}, {5, 50}};

    const auto it_0 = m.find(0);
    BOOST_CHECK(it_0 == m.end());

    const auto it_1 = m.find(1);
    BOOST_CHECK(it_1 != m.end());
    BOOST_CHECK(*it_1 == std::make_pair(1, 10));
    const auto it_2 = m.find(2);
    BOOST_CHECK(it_2 != m.end());
    BOOST_CHECK(*it_2 == std::make_pair(2, 20));
    const auto it_3 = m.find(3);
    BOOST_CHECK(it_3 != m.end());
    BOOST_CHECK(*it_3 == std::make_pair(3, 30));
    const auto it_4 = m.find(4);
    BOOST_CHECK(it_4 != m.end());
    BOOST_CHECK(*it_4 == std::make_pair(4, 40));
    const auto it_5 = m.find(5);
    BOOST_CHECK(it_5 != m.end());
    BOOST_CHECK(*it_5 == std::make_pair(5, 50));

    const auto it_6 = m.find(6);
    BOOST_CHECK(it_6 == m.end());
}

BOOST_AUTO_TEST_CASE(nonempty__hash_map_of_string_int__find) {
    stuff::container::hash_map<std::string, int> m = {
        {"1"s, 10}, {"2"s, 20}, {"3"s, 30}, {"4"s, 40}, {"5"s, 50}};

    const auto it_0 = m.find("0"s);
    BOOST_CHECK(it_0 == m.end());

    const auto it_1 = m.find("1"s);
    BOOST_CHECK(it_1 != m.end());
    BOOST_CHECK(*it_1 == std::make_pair("1"s, 10));
    const auto it_2 = m.find("2"s);
    BOOST_CHECK(it_2 != m.end());
    BOOST_CHECK(*it_2 == std::make_pair("2"s, 20));
    const auto it_3 = m.find("3"s);
    BOOST_CHECK(it_3 != m.end());
    BOOST_CHECK(*it_3 == std::make_pair("3"s, 30));
    const auto it_4 = m.find("4"s);
    BOOST_CHECK(it_4 != m.end());
    BOOST_CHECK(*it_4 == std::make_pair("4"s, 40));
    const auto it_5 = m.find("5"s);
    BOOST_CHECK(it_5 != m.end());
    BOOST_CHECK(*it_5 == std::make_pair("5"s, 50));

    const auto it_6 = m.find("6"s);
    BOOST_CHECK(it_6 == m.end());
}

BOOST_AUTO_TEST_CASE(hash_map_of_int_int__iterator_distance) {
    stuff::container::hash_map<int, int> m = {{1, 10}, {2, 20}, {3, 30}, {4, 40},
                                              {5, 50}, {1, 11}, {2, 21}, {3, 31},
                                              {4, 41}, {5, 51}};
    BOOST_CHECK_EQUAL(std::distance(m.begin(), m.end()), 5);
}

BOOST_AUTO_TEST_CASE(hash_map_of_string_int__iterator_distance) {
    stuff::container::hash_map<std::string, int> m = {
        {"1"s, 10}, {"2"s, 20}, {"3"s, 30}, {"4"s, 40}, {"5"s, 50},
        {"1"s, 11}, {"2"s, 21}, {"3"s, 31}, {"4"s, 41}, {"5"s, 51}};
    BOOST_CHECK_EQUAL(std::distance(m.begin(), m.end()), 5);
}
