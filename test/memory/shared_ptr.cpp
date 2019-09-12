#define BOOST_TEST_MODULE memory__shared_ptr__test

#include <boost/test/included/unit_test.hpp>
#include <stuff/memory/shared_ptr.hpp>
#include <stuff/util/test/types.hpp>

BOOST_AUTO_TEST_CASE(single_threaded__copy__int) {
    stuff::shared_ptr<int, stuff::single_threaded<true>> pi0{new int(1)};
    stuff::shared_ptr<int, stuff::single_threaded<true>> pi1;
    pi1 = pi0;
    BOOST_CHECK_EQUAL(pi1.get(), pi0.get());

    stuff::shared_ptr<int, stuff::single_threaded<true>> pi2;
    pi1 = pi2;
    BOOST_CHECK_EQUAL(pi1.get(), pi2.get());
}

BOOST_AUTO_TEST_CASE(single_threaded__release__int) {
    stuff::shared_ptr<int, stuff::single_threaded<true>> pi;
    pi.reset();
    BOOST_CHECK_EQUAL(pi.get(), nullptr);
}

BOOST_AUTO_TEST_CASE(single_threaded__make_shared__int) {
    const int value = 100500;
    auto pint = stuff::make_shared<int, stuff::single_threaded<true>>(value);
    BOOST_CHECK(pint.get());
    BOOST_CHECK_EQUAL(*pint, value);
}

class counted : public stuff::util::test::constructions_counter, public stuff::util::test::destructions_counter {};

DECLARE_CONSTRUCTIONS_COUNTER
DECLARE_DESTRUCTIONS_COUNTER

BOOST_AUTO_TEST_CASE(single_threaded__copy__counted) {
    counted::constructions_counter::reset();
    counted::destructions_counter::reset();
    stuff::shared_ptr<counted, stuff::single_threaded<true>> p0{new counted};
    stuff::shared_ptr<counted, stuff::single_threaded<true>> p1;
    p1 = p0;
    BOOST_CHECK_EQUAL(p1.get(), p0.get());

    stuff::shared_ptr<counted, stuff::single_threaded<true>> p2;
    p1 = p2;
    BOOST_CHECK_EQUAL(p1.get(), p2.get());
    BOOST_CHECK_EQUAL(counted::constructions_counter::value(), 1);
    BOOST_CHECK_EQUAL(counted::destructions_counter::value(), 1);
}


BOOST_AUTO_TEST_CASE(single_threaded__make_shared__counted) {
    counted::constructions_counter::reset();
    counted::destructions_counter::reset();
    {
        auto ptr = stuff::make_shared<counted, stuff::single_threaded<true>>();
    }
    BOOST_CHECK_EQUAL(counted::constructions_counter::value(), 1);
    BOOST_CHECK_EQUAL(counted::destructions_counter::value(), 1);
}