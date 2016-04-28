#include <vanetza/facilities/path_point.hpp>
#include <gtest/gtest.h>

namespace units = vanetza::units;
using vanetza::facilities::PathPoint;

TEST(PathPoint, chord_length) {
    PathPoint a;
    a.longitude = 12.14094444 * units::degrees;
    a.latitude = 49.53858333 * units::degrees;

    PathPoint b;
    b.longitude = 12.27191666 * units::degrees;
    b.latitude = 49.72394444 * units::degrees;

    EXPECT_EQ(chord_length(a, b), chord_length(b, a));

    units::Length length = chord_length(a, b);
    EXPECT_NEAR(22692.54, length / units::si::meters, 0.01);
}
