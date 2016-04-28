#include <gtest/gtest.h>
#include <vanetza/geonet/timestamp.hpp>
#include <limits>

using namespace vanetza::geonet;
const Timestamp::absolute_unit_type abs_ms;
const auto max = std::numeric_limits<Timestamp::value_type>::max();

TEST(Timestamp, ctor) {
    Timestamp a;
    EXPECT_EQ(a.raw(), 0);

    Timestamp b(Timestamp::time_type::from_value(832));
    EXPECT_EQ(b.raw(), 832);

    boost::posix_time::ptime p =
        boost::posix_time::time_from_string("2004-01-01 00:00:00.000");
    Timestamp c(p);
    EXPECT_EQ(c.raw(), 0);

    boost::posix_time::ptime q = p + boost::posix_time::milliseconds(5234);
    Timestamp d(q);
    EXPECT_EQ(d.raw(), 5234);
}

TEST(Timestamp, equality) {
    Timestamp a(413 * abs_ms);
    Timestamp b(412 * abs_ms);
    EXPECT_NE(a, b);

    Timestamp c(a);
    EXPECT_EQ(a, c);
}

TEST(Timestamp, max) {
    Timestamp a(895723 * abs_ms);
    Timestamp b((895723 + max + 1) * abs_ms);
    EXPECT_EQ(a, b);
}

TEST(Timestamp, is_greater) {
    Timestamp a(238 * abs_ms);
    Timestamp b(513 * abs_ms);
    EXPECT_TRUE(is_greater(b, a));
    EXPECT_FALSE(is_greater(a, b));

    Timestamp c((513 + max / 2) * abs_ms);
    EXPECT_TRUE(is_greater(c, b));
    Timestamp d((514 + max / 2) * abs_ms);
    EXPECT_FALSE(is_greater(d, b));

    Timestamp e(Timestamp::time_type::from_value(max));
    EXPECT_FALSE(is_greater(e, a));
    Timestamp f(Timestamp::time_type::from_value(max/2));
    EXPECT_FALSE(is_greater(e, f));
    Timestamp g(Timestamp::time_type::from_value(max/2 + 1));
    EXPECT_TRUE(is_greater(e, g));

    Timestamp h(513 * abs_ms);
    EXPECT_EQ(b, h);
    EXPECT_FALSE(is_greater(b, h));
}

TEST(Timestamp, less) {
    // operator< uses is_greater: we don't have to test a lot of combinations here
    Timestamp a(582 * abs_ms);
    Timestamp b(54884 * abs_ms);
    EXPECT_LT(a, b);
    Timestamp c(54884 * abs_ms);
    EXPECT_FALSE(c < b);
}

TEST(Timestamp, addition) {
    Timestamp a(89 * abs_ms);
    a += 348 * Timestamp::millisecond();
    EXPECT_EQ(a.raw(), 437);

    Timestamp b = a + 3483 * Timestamp::millisecond();
    EXPECT_EQ(a.raw(), 437);
    EXPECT_EQ(b.raw(), 3920);
}

TEST(Timestamp, substraction) {
    Timestamp a(3483 * abs_ms);
    a -= 89 * Timestamp::millisecond();
    EXPECT_EQ(a.raw(), 3394);
    a -= 5000 * Timestamp::millisecond();
    EXPECT_EQ(a.raw(), 4294965690);

    Timestamp b = a - 23 * Timestamp::millisecond();
    EXPECT_EQ(a.raw(), 4294965690);
    EXPECT_EQ(b.raw(), 4294965667);
}

TEST(Timestamp, difference) {
    Timestamp a(329 * abs_ms);
    Timestamp b(394 * abs_ms);
    EXPECT_EQ((a - b).value(), 4294967231);
    EXPECT_EQ((b - a).value(), 65);
}

