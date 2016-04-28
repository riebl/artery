#include <gtest/gtest.h>
#include <vanetza/units/time.hpp>
#include <boost/units/systems/si/prefixes.hpp>
#include <chrono>

using namespace vanetza::units;
namespace sc = std::chrono;

TEST(Time, clock_cast_from_quantity)
{
    EXPECT_EQ(sc::milliseconds(3), clock_cast(3.0 * si::milli * si::seconds));
    EXPECT_EQ(sc::seconds(354), clock_cast(354 * si::seconds));
}

TEST(Time, clock_cast_from_chrono)
{
    EXPECT_EQ(0.5 * si::seconds, clock_cast(sc::milliseconds(500)));
    EXPECT_EQ(7200 * si::seconds, clock_cast(sc::hours(2)));
}
