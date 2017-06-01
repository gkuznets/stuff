#define BOOST_TEST_MODULE container__hash_set__test

#include <boost/test/included/unit_test.hpp>
#include <stuff/container/hash_set.hpp>

#include <iterator>
#include <string>
#include <vector>

using namespace std::string_literals;
namespace sc = stuff::container;

BOOST_AUTO_TEST_CASE(hash_set_of_int__operator_copy) {
    sc::hash_set<int> s1 = {1, 2, 3};
    sc::hash_set<int> s2 = {4, 5, 6};
    sc::hash_set<int> s3;

    s3 = s1;
    BOOST_CHECK_EQUAL(s3.count(1), 1);
    BOOST_CHECK_EQUAL(s3.count(2), 1);
    BOOST_CHECK_EQUAL(s3.count(3), 1);
    s1.clear();
    BOOST_CHECK_EQUAL(s3.count(1), 1);
    BOOST_CHECK_EQUAL(s3.count(2), 1);
    BOOST_CHECK_EQUAL(s3.count(3), 1);

    s3 = s2;
    BOOST_CHECK_EQUAL(s3.count(4), 1);
    BOOST_CHECK_EQUAL(s3.count(5), 1);
    BOOST_CHECK_EQUAL(s3.count(6), 1);
    s3.clear();
    BOOST_CHECK_EQUAL(s2.count(4), 1);
    BOOST_CHECK_EQUAL(s2.count(5), 1);
    BOOST_CHECK_EQUAL(s2.count(6), 1);
}

BOOST_AUTO_TEST_CASE(hash_set_of_string__operator_copy) {
    sc::hash_set<std::string> s1 = {"1"s, "2"s, "3"s};
    const sc::hash_set<std::string> s2 = {"4"s, "5"s, "6"s};
    sc::hash_set<std::string> s3;

    s3 = s1;
    BOOST_CHECK_EQUAL(s3.count("1"s), 1);
    BOOST_CHECK_EQUAL(s3.count("2"s), 1);
    BOOST_CHECK_EQUAL(s3.count("3"s), 1);
    s1.clear();
    BOOST_CHECK_EQUAL(s3.count("1"s), 1);
    BOOST_CHECK_EQUAL(s3.count("2"s), 1);
    BOOST_CHECK_EQUAL(s3.count("3"s), 1);

    s3 = s2;
    BOOST_CHECK_EQUAL(s3.count("4"s), 1);
    BOOST_CHECK_EQUAL(s3.count("5"s), 1);
    BOOST_CHECK_EQUAL(s3.count("6"s), 1);
    s3.clear();
    BOOST_CHECK_EQUAL(s2.count("4"s), 1);
    BOOST_CHECK_EQUAL(s2.count("5"s), 1);
    BOOST_CHECK_EQUAL(s2.count("6"s), 1);
}

BOOST_AUTO_TEST_CASE(hash_set_of_int__empty) {
    sc::hash_set<int> s;
    BOOST_CHECK(s.empty());
    s.insert(1);
    BOOST_CHECK(!s.empty());
}

BOOST_AUTO_TEST_CASE(hash_set_of_string__empty) {
    sc::hash_set<std::string> s;
    BOOST_CHECK(s.empty());
    s.insert("1"s);
    BOOST_CHECK(!s.empty());
}

BOOST_AUTO_TEST_CASE(hash_set_of_int__size) {
    sc::hash_set<int> s;
    BOOST_CHECK_EQUAL(s.size(), 0);
    s.insert(1);
    BOOST_CHECK_EQUAL(s.size(), 1);
    s.insert(2);
    BOOST_CHECK_EQUAL(s.size(), 2);
}

BOOST_AUTO_TEST_CASE(hash_set_of_string__size) {
    sc::hash_set<std::string> s;
    BOOST_CHECK_EQUAL(s.size(), 0);
    s.insert("1"s);
    BOOST_CHECK_EQUAL(s.size(), 1);
    s.insert("2"s);
    BOOST_CHECK_EQUAL(s.size(), 2);
}

BOOST_AUTO_TEST_CASE(hash_set_of_int__clear) {
    sc::hash_set<int> s;
    s.clear();
    BOOST_CHECK(s.empty());
    s.insert(1);
    s.insert(2);
    s.insert(3);
    s.insert(4);
    s.clear();
    BOOST_CHECK(s.empty());
}

BOOST_AUTO_TEST_CASE(hash_set_of_string__clear) {
    sc::hash_set<std::string> s;
    s.clear();
    BOOST_CHECK(s.empty());
    s.insert("1"s);
    s.insert("2"s);
    s.insert("3"s);
    s.insert("4"s);
    s.clear();
    BOOST_CHECK(s.empty());
}

BOOST_AUTO_TEST_CASE(hash_set_of_int__insert) {
    sc::hash_set<int> s;
    auto ret = s.insert(1);
    BOOST_CHECK(ret.second);
    BOOST_CHECK_EQUAL(*ret.first, 1);

    ret = s.insert(1);
    BOOST_CHECK(!ret.second);
    BOOST_CHECK_EQUAL(*ret.first, 1);
}

BOOST_AUTO_TEST_CASE(hash_set_of_string__insert) {
    sc::hash_set<std::string> s;
    auto ret = s.insert("1"s);
    BOOST_CHECK(ret.second);
    BOOST_CHECK_EQUAL(*ret.first, "1"s);

    ret = s.insert("1"s);
    BOOST_CHECK(!ret.second);
    BOOST_CHECK_EQUAL(*ret.first, "1"s);
}

BOOST_AUTO_TEST_CASE(hash_set_of_int__erase_iter) {
    sc::hash_set<int> s = {1, 2, 3};
    auto remaining_elements = s.size();
    for (auto it = s.begin(); it != s.end();) {
        auto value = *it;
        it = s.erase(it);
        BOOST_CHECK_EQUAL(s.count(value), 0);
        if (--remaining_elements > 0) {
            BOOST_CHECK(it != s.end());
        } else {
            BOOST_CHECK(it == s.end());
        }
    }
    BOOST_CHECK_EQUAL(remaining_elements, 0);
}

BOOST_AUTO_TEST_CASE(hash_set_of_string__erase_iter) {
    sc::hash_set<std::string> s = {"1"s, "2"s, "3"s};
    auto remaining_elements = s.size();
    for (auto it = s.begin(); it != s.end();) {
        auto value = *it;
        it = s.erase(it);
        BOOST_CHECK_EQUAL(s.count(value), 0);
        if (--remaining_elements > 0) {
            BOOST_CHECK(it != s.end());
        } else {
            BOOST_CHECK(it == s.end());
        }
    }
    BOOST_CHECK_EQUAL(remaining_elements, 0);
}

BOOST_AUTO_TEST_CASE(hash_set_of_int__erase_range) {
    sc::hash_set<int> s = {1, 2, 3, 4, 5};
    std::vector<int> to_be_removed{std::next(s.begin()), std::next(s.begin(), 4)};
    s.erase(std::next(s.begin()), std::next(s.begin(), 4));
    for (const auto& x : to_be_removed) {
        BOOST_CHECK_EQUAL(s.count(x), 0);
    }
}

BOOST_AUTO_TEST_CASE(hash_set_of_string__erase_range) {
    sc::hash_set<std::string> s = {"1"s, "2"s, "3"s, "4"s, "5"s};
    std::vector<std::string> to_be_removed{std::next(s.begin()), std::next(s.begin(), 4)};
    s.erase(std::next(s.begin()), std::next(s.begin(), 4));
    for (const auto& x : to_be_removed) {
        BOOST_CHECK_EQUAL(s.count(x), 0);
    }
}

BOOST_AUTO_TEST_CASE(hash_set_of_int__erase_value) {
    sc::hash_set<int> s;
    s.insert(1);
    BOOST_CHECK_EQUAL(s.erase(0), 0);
    BOOST_CHECK_EQUAL(s.erase(1), 1);
    BOOST_CHECK_EQUAL(s.erase(1), 0);
}

BOOST_AUTO_TEST_CASE(hash_set_of_string__erase_value) {
    sc::hash_set<std::string> s;
    s.insert("1"s);
    BOOST_CHECK_EQUAL(s.erase("0"s), 0);
    BOOST_CHECK_EQUAL(s.erase("1"s), 1);
    BOOST_CHECK_EQUAL(s.erase("1"s), 0);
}

BOOST_AUTO_TEST_CASE(empty__hash_set_of_int__count) {
    sc::hash_set<int> s;
    BOOST_CHECK_EQUAL(s.count(1), 0);
}

BOOST_AUTO_TEST_CASE(empty__hash_set_of_string__count) {
    sc::hash_set<std::string> s;
    BOOST_CHECK_EQUAL(s.count("1"s), 0);
}

BOOST_AUTO_TEST_CASE(nonempty__hash_set_of_int__count) {
    sc::hash_set<int> s = {1, 2, 3, 4, 5};

    BOOST_CHECK_EQUAL(s.count(0), 0);
    BOOST_CHECK_EQUAL(s.count(1), 1);
    BOOST_CHECK_EQUAL(s.count(2), 1);
    BOOST_CHECK_EQUAL(s.count(3), 1);
    BOOST_CHECK_EQUAL(s.count(4), 1);
    BOOST_CHECK_EQUAL(s.count(5), 1);
    BOOST_CHECK_EQUAL(s.count(6), 0);
}

BOOST_AUTO_TEST_CASE(nonempty__hash_set_of_string__count) {
    sc::hash_set<std::string> s = {"1"s, "2"s, "3"s, "4"s, "5"s};

    BOOST_CHECK_EQUAL(s.count("0"s), 0);
    BOOST_CHECK_EQUAL(s.count("1"s), 1);
    BOOST_CHECK_EQUAL(s.count("2"s), 1);
    BOOST_CHECK_EQUAL(s.count("3"s), 1);
    BOOST_CHECK_EQUAL(s.count("4"s), 1);
    BOOST_CHECK_EQUAL(s.count("5"s), 1);
    BOOST_CHECK_EQUAL(s.count("6"s), 0);
}

BOOST_AUTO_TEST_CASE(empty__hash_set_of_int__find) {
    sc::hash_set<int> s;
    BOOST_CHECK(s.find(1) == s.end());
}

BOOST_AUTO_TEST_CASE(empty__hash_set_of_string__find) {
    sc::hash_set<std::string> s;
    BOOST_CHECK(s.find("1"s) == s.end());
}

BOOST_AUTO_TEST_CASE(nonempty__hash_set_of_int__find) {
    sc::hash_set<int> s = {1, 2, 3, 4, 5};

    const auto it_0 = s.find(0);
    BOOST_CHECK(it_0 == s.end());

    const auto it_1 = s.find(1);
    BOOST_CHECK(it_1 != s.end());
    BOOST_CHECK_EQUAL(*it_1, 1);
    const auto it_2 = s.find(2);
    BOOST_CHECK(it_2 != s.end());
    BOOST_CHECK_EQUAL(*it_2, 2);
    const auto it_3 = s.find(3);
    BOOST_CHECK(it_3 != s.end());
    BOOST_CHECK_EQUAL(*it_3, 3);
    const auto it_4 = s.find(4);
    BOOST_CHECK(it_4 != s.end());
    BOOST_CHECK_EQUAL(*it_4, 4);
    const auto it_5 = s.find(5);
    BOOST_CHECK(it_5 != s.end());
    BOOST_CHECK_EQUAL(*it_5, 5);

    const auto it_6 = s.find(6);
    BOOST_CHECK(it_6 == s.end());
}

BOOST_AUTO_TEST_CASE(nonempty__hash_set_of_string__find) {
    sc::hash_set<std::string> s = {"1"s, "2"s, "3"s, "4"s, "5"s};

    const auto it_0 = s.find("0"s);
    BOOST_CHECK(it_0 == s.end());

    const auto it_1 = s.find("1"s);
    BOOST_CHECK(it_1 != s.end());
    BOOST_CHECK_EQUAL(*it_1, "1"s);
    const auto it_2 = s.find("2"s);
    BOOST_CHECK(it_2 != s.end());
    BOOST_CHECK_EQUAL(*it_2, "2"s);
    const auto it_3 = s.find("3"s);
    BOOST_CHECK(it_3 != s.end());
    BOOST_CHECK_EQUAL(*it_3, "3"s);
    const auto it_4 = s.find("4"s);
    BOOST_CHECK(it_4 != s.end());
    BOOST_CHECK_EQUAL(*it_4, "4"s);
    const auto it_5 = s.find("5"s);
    BOOST_CHECK(it_5 != s.end());
    BOOST_CHECK_EQUAL(*it_5, "5"s);

    const auto it_6 = s.find("6"s);
    BOOST_CHECK(it_6 == s.end());
}

BOOST_AUTO_TEST_CASE(hash_set_of_int__iterator_distance) {
    sc::hash_set<int> s = {1, 2, 3, 4, 5, 1, 2, 3, 4, 5};
    BOOST_CHECK_EQUAL(std::distance(s.begin(), s.end()), 5);
}

BOOST_AUTO_TEST_CASE(hash_set_of_string__iterator_distance) {
    sc::hash_set<std::string> s = {"1"s, "2"s, "3"s, "4"s, "5"s,
                                   "1"s, "2"s, "3"s, "4"s, "5"s};
    BOOST_CHECK_EQUAL(std::distance(s.begin(), s.end()), 5);
}

BOOST_AUTO_TEST_CASE(hash_set_of_int__mix_of_insertions_and_removals) {
    std::vector<int> numbers;
    for (int i = 0; i < 3 * 100 + 1; ++i) {
        numbers.push_back(i);
    }

    sc::hash_set<int> s;
    for (int op = 0; op < 2000; ++op) {
        const int x = numbers[(op * 3) % numbers.size()];
        const bool present = s.count(x) == 1;
        if (op % 2 == 0) {
            const auto ret = s.insert(x);
            BOOST_CHECK_EQUAL(*ret.first, x);
            BOOST_CHECK_EQUAL(ret.second, !present);
        } else {
            const auto size = s.size();
            s.erase(x);
            BOOST_CHECK(!present || s.size() == size - 1);
            BOOST_CHECK_EQUAL(s.count(x), 0);
        }
    }
}

BOOST_AUTO_TEST_CASE(hash_set_of_string__mix_of_insertions_and_removals) {
    std::vector<std::string> strings;
    for (int i = 0; i < 3 * 100 + 1; ++i) {
        strings.push_back(std::to_string(i));
    }

    sc::hash_set<std::string> s;
    for (int op = 0; op < 2000; ++op) {
        const auto x = strings[(op * 3) % strings.size()];
        const bool present = s.count(x) == 1;
        const auto size = s.size();
        if (op % 2 == 0) {
            const auto ret = s.insert(x);
            BOOST_CHECK_EQUAL(*ret.first, x);
            BOOST_CHECK_EQUAL(ret.second, !present);
            BOOST_CHECK(present || s.size() == size + 1);
        } else {
            s.erase(x);
            BOOST_CHECK(!present || s.size() == size - 1);
            BOOST_CHECK_EQUAL(s.count(x), 0);
        }
    }
}

BOOST_AUTO_TEST_CASE(hash_set_of_int__equality) {
    sc::hash_set<int> s1;
    sc::hash_set<int> s2;
    BOOST_CHECK(s1 == s2);

    for (int i = 0; i < 100; ++i) {
        s1.insert(i);
        BOOST_CHECK(s1 != s2);
    }
    for (int i = 99; i >= 0; --i) {
        BOOST_CHECK(s1 != s2);
        s2.insert(i);
    }
    BOOST_CHECK(s1 == s2);
}

BOOST_AUTO_TEST_CASE(hash_set_of_string__equality) {
    sc::hash_set<std::string> s1;
    sc::hash_set<std::string> s2;
    BOOST_CHECK(s1 == s2);

    for (int i = 0; i < 100; ++i) {
        s1.insert(std::to_string(i));
        BOOST_CHECK(s1 != s2);
    }
    for (int i = 99; i >= 0; --i) {
        BOOST_CHECK(s1 != s2);
        s2.insert(std::to_string(i));
    }
    BOOST_CHECK(s1 == s2);
}
