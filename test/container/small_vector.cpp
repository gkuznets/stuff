#define BOOST_TEST_MODULE container__small_vector__test

#include <algorithm>
#include <iterator>
#include <list>

#include <boost/test/included/unit_test.hpp>
#include <stuff/container/small_vector.hpp>
#include <stuff/types.hpp>
#include <stuff/util/test/types.hpp>

namespace sut = stuff::util::test;
DECLARE_CONSTRUCTIONS_COUNTER
DECLARE_DESTRUCTIONS_COUNTER

BOOST_AUTO_TEST_CASE(default_constructor) {
    stuff::container::small_vector<int, 10> sv;
    BOOST_CHECK(sv.empty());
    BOOST_CHECK_EQUAL(sv.capacity(), 10);
}

BOOST_AUTO_TEST_CASE(constructor_from_pair_of_iterators) {
    const std::list<int> l = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    {
        stuff::container::small_vector<int, 9> sv{l.begin(), l.end()};
        BOOST_CHECK(std::equal(l.begin(), l.end(), sv.begin(), sv.end()));
    }
    {
        stuff::container::small_vector<int, 10> sv{l.begin(), l.end()};
        BOOST_CHECK(std::equal(l.begin(), l.end(), sv.begin(), sv.end()));
    }
    {
        stuff::container::small_vector<int, 11> sv{l.begin(), l.end()};
        BOOST_CHECK(std::equal(l.begin(), l.end(), sv.begin(), sv.end()));
    }
}

BOOST_AUTO_TEST_CASE(copy_constructor) {
    stuff::container::small_vector<int, 10> sv = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    {
        stuff::container::small_vector<int, 9> sv_copy{sv};
        BOOST_CHECK(sv.begin() != sv_copy.begin());
        BOOST_CHECK(sv == sv_copy);
    }
    {
        stuff::container::small_vector<int, 10> sv_copy{sv};
        BOOST_CHECK(sv.begin() != sv_copy.begin());
        BOOST_CHECK(sv == sv_copy);
    }
    {
        stuff::container::small_vector<int, 11> sv_copy{sv};
        BOOST_CHECK(sv.begin() != sv_copy.begin());
        BOOST_CHECK(sv == sv_copy);
    }
    sv.push_back(10);
    {
        stuff::container::small_vector<int, 10> sv_copy{sv};
        BOOST_CHECK(sv.begin() != sv_copy.begin());
        BOOST_CHECK(sv == sv_copy);
    }
    {
        stuff::container::small_vector<int, 11> sv_copy{sv};
        BOOST_CHECK(sv.begin() != sv_copy.begin());
        BOOST_CHECK(sv == sv_copy);
    }
    {
        stuff::container::small_vector<int, 12> sv_copy{sv};
        BOOST_CHECK(sv.begin() != sv_copy.begin());
        BOOST_CHECK(sv == sv_copy);
    }
}

BOOST_AUTO_TEST_CASE(at) {
    stuff::container::small_vector<int, 10> sv;
    BOOST_CHECK_THROW(sv.at(0), std::out_of_range);
    sv.push_back(1);
    BOOST_CHECK_EQUAL(sv.at(0), 1);
    BOOST_CHECK_THROW(sv.at(1), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(subscript_operator) {
    stuff::container::small_vector<int, 10> sv;
    sv.push_back(1);
    sv.push_back(2);
    BOOST_CHECK_EQUAL(sv[0], 1);
    BOOST_CHECK_EQUAL(sv[1], 2);
}

BOOST_AUTO_TEST_CASE(front) {
    stuff::container::small_vector<int, 10> sv;
    sv.push_back(1);
    sv.push_back(2);
    BOOST_CHECK_EQUAL(sv.front(), 1);
}

BOOST_AUTO_TEST_CASE(back) {
    stuff::container::small_vector<int, 10> sv;
    sv.push_back(1);
    sv.push_back(2);
    BOOST_CHECK_EQUAL(sv.back(), 2);
}

BOOST_AUTO_TEST_CASE(clear) {
    stuff::container::small_vector<int, 10> sv;
    sv.clear();
    BOOST_CHECK(sv.empty());
    BOOST_CHECK_EQUAL(sv.capacity(), 10);

    sv.push_back(1);
    sv.clear();
    BOOST_CHECK(sv.empty());
    BOOST_CHECK_EQUAL(sv.capacity(), 10);

    for (int i = 1; i < 30; ++i) {
        sv.push_back(i);
    }
    const auto capacity = sv.capacity();
    sv.clear();
    BOOST_CHECK(sv.empty());
    BOOST_CHECK_EQUAL(sv.capacity(), capacity);
}

BOOST_AUTO_TEST_CASE(size) {
    stuff::container::small_vector<int, 10> sv;
    for (int i = 0; i < 30; ++i) {
        BOOST_CHECK_EQUAL(sv.size(), i);
        sv.push_back(i);
    }
}

BOOST_AUTO_TEST_CASE(resize) {
    {
        stuff::container::small_vector<int, 10> sv;
        sv.push_back(1);
        sv.resize(1);
        BOOST_CHECK_EQUAL(sv.size(), 1);
        sv.resize(0);
        BOOST_CHECK_EQUAL(sv.size(), 0);
    }
}

BOOST_AUTO_TEST_CASE(erase) {
    {
        stuff::container::small_vector<int, 10> sv = {0, 1, 2, 3, 4, 100500,
                                                      5, 6, 7, 8, 9};
        const auto initial_size = sv.size();
        const auto it = sv.erase(std::next(sv.begin(), 5));
        BOOST_CHECK_EQUAL(sv.size(), initial_size - 1);
        BOOST_CHECK_EQUAL(*it, 5);
        for (int i = 0; i < sv.size(); ++i) {
            BOOST_CHECK_EQUAL(sv[i], i);
        }
    }
    {
        stuff::container::small_vector<int, 10> sv = {0, 1, 2, 3, 4, 100500,
                                                      5, 6, 7, 8, 9};
        const auto initial_size = sv.size();
        const auto it = sv.erase(std::next(sv.begin(), 5), std::next(sv.begin(), 6));
        BOOST_CHECK_EQUAL(sv.size(), initial_size - 1);
        BOOST_CHECK_EQUAL(*it, 5);
        for (int i = 0; i < sv.size(); ++i) {
            BOOST_CHECK_EQUAL(sv[i], i);
        }
    }
}

BOOST_AUTO_TEST_CASE(push_back) {
    stuff::container::small_vector<int, 10> sv;

    for (int i = 1; i < 30; ++i) {
        sv.push_back(1);
        BOOST_CHECK(!sv.empty());
        BOOST_CHECK_EQUAL(sv.size(), i);
    }
}

BOOST_AUTO_TEST_CASE(pop_back) {
    stuff::container::small_vector<int, 10> sv;
    sv.push_back(1);
    sv.push_back(2);
    sv.push_back(3);
    sv.pop_back();
    BOOST_CHECK_EQUAL(sv.back(), 2);
}

// Memory management tests
BOOST_AUTO_TEST_CASE(resize__memory_mgmt) {
    using elem = sut::type<sut::constructions_counter, sut::destructions_counter>;
    stuff::container::small_vector<elem, 10> sv;
    BOOST_CHECK_EQUAL(sut::constructions_counter::value(), 0);
    BOOST_CHECK_EQUAL(sut::destructions_counter::value(), 0);
    sv.resize(5);
    BOOST_CHECK_EQUAL(sut::constructions_counter::value(), 5);
    sv.resize(10);
    BOOST_CHECK_EQUAL(sut::constructions_counter::value(), 10);
    sv.resize(20);
    BOOST_CHECK_EQUAL(sut::constructions_counter::value() - sv.size(),
                      sut::destructions_counter::value());
    sv.resize(30);
    BOOST_CHECK_EQUAL(sut::constructions_counter::value() - sv.size(),
                      sut::destructions_counter::value());
    sv.resize(20);
    BOOST_CHECK_EQUAL(sut::constructions_counter::value() - sv.size(),
                      sut::destructions_counter::value());
    sv.clear();
    BOOST_CHECK_EQUAL(sut::constructions_counter::value(),
                      sut::destructions_counter::value());

    sut::constructions_counter::reset();
    sut::destructions_counter::reset();
}

bool aligned_at(const void* ptr, unsigned alignment) {
    return reinterpret_cast<stuff::uint_t<sizeof(ptr) * 8>>(ptr) % alignment == 0;
}

BOOST_AUTO_TEST_CASE(alignment) {
    {
        stuff::container::small_vector<sut::type<sut::alignment<1>>, 10> sv{1};
        BOOST_CHECK(aligned_at(std::addressof(sv[0]), 1));
    }
    {
        stuff::container::small_vector<sut::type<sut::alignment<2>>, 10> sv{1};
        BOOST_CHECK(aligned_at(std::addressof(sv[0]), 2));
    }
    {
        stuff::container::small_vector<sut::type<sut::alignment<4>>, 10> sv{1};
        BOOST_CHECK(aligned_at(std::addressof(sv[0]), 4));
    }
    {
        stuff::container::small_vector<sut::type<sut::alignment<8>>, 10> sv{1};
        BOOST_CHECK(aligned_at(std::addressof(sv[0]), 8));
    }
    {
        stuff::container::small_vector<sut::type<sut::alignment<16>>, 10> sv{1};
        BOOST_CHECK(aligned_at(std::addressof(sv[0]), 16));
    }
    {
        stuff::container::small_vector<sut::type<sut::alignment<32>>, 10> sv{1};
        BOOST_CHECK(aligned_at(std::addressof(sv[0]), 32));
    }
}
