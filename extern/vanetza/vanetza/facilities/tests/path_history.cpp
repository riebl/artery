#include <vanetza/facilities/path_history.hpp>
#include <gtest/gtest.h>

using vanetza::facilities::PathHistory;
using vanetza::facilities::PathPoint;
namespace units = vanetza::units;

#define EXPECT_PATHPOINT_EQ(a, b) \
    EXPECT_DOUBLE_EQ(a.latitude.value(), b.latitude.value()); \
    EXPECT_DOUBLE_EQ(a.longitude.value(), b.longitude.value()); \
    EXPECT_DOUBLE_EQ(a.heading.value(), b.heading.value()); \
    EXPECT_EQ(a.time, b.time)

const units::GeoAngle cOneMeterLatitude = 1.0 / 111320.0 * units::degrees;

TEST(PathHistory, reference_point) {
    PathHistory ph;
    const PathPoint default_pp;
    EXPECT_PATHPOINT_EQ(default_pp, ph.getReferencePoint());

    PathPoint first_pp;
    first_pp.longitude = 34.4 * units::degrees;
    first_pp.latitude = -10.3 * units::degrees;
    first_pp.heading = units::Angle { 48.3 * units::degrees };
    first_pp.time = boost::posix_time::time_from_string("2014-11-21 11:11:48");

    ph.addSample(first_pp);
    EXPECT_PATHPOINT_EQ(first_pp, ph.getReferencePoint());

    PathPoint second_pp;
    second_pp.longitude = 34.4 * units::degrees;
    second_pp.latitude = -10.3 * units::degrees;
    second_pp.heading = units::Angle { 48.3 * units::degrees };
    second_pp.time = boost::posix_time::time_from_string("2014-11-21 11:11:48.1");

    ph.addSample(second_pp);
    EXPECT_PATHPOINT_EQ(second_pp, ph.getReferencePoint());
}

TEST(PathHistory, concise_points_of_equal_samples) {
    PathHistory ph;
    EXPECT_EQ(0, ph.getConcisePoints().size());

    PathPoint first_pp;
    first_pp.longitude = -3.45 * units::degrees;
    first_pp.latitude = 32.98 * units::degrees;
    ph.addSample(first_pp);
    ASSERT_EQ(1, ph.getConcisePoints().size());
    EXPECT_PATHPOINT_EQ(first_pp, ph.getConcisePoints().front());

    for (unsigned i = 0; i < 10; ++i) {
        ph.addSample(first_pp);
        EXPECT_EQ(1, ph.getConcisePoints().size());
    }
}

TEST(PathHistory, concise_points_chord_length_threshold) {
    PathHistory ph;

    PathPoint pp;
    pp.longitude = 0.0 * units::degrees;
    pp.latitude = 0.0  * units::degrees;
    ph.addSample(pp);
    pp.latitude += cOneMeterLatitude;
    ph.addSample(pp);
    pp.latitude += cOneMeterLatitude;
    ph.addSample(pp);
    EXPECT_EQ(1, ph.getConcisePoints().size());

    pp.latitude += cOneMeterLatitude;
    ph.addSample(pp);
    EXPECT_EQ(1, ph.getConcisePoints().size());

    pp.latitude += 20.0 * cOneMeterLatitude;
    ph.addSample(pp);
    ASSERT_EQ(2, ph.getConcisePoints().size());
    EXPECT_DOUBLE_EQ(3.0 * cOneMeterLatitude.value(),
            ph.getConcisePoints().front().latitude.value());

    pp.latitude += 10.0 * cOneMeterLatitude;
    ph.addSample(pp);
    EXPECT_EQ(3, ph.getConcisePoints().size());
    EXPECT_DOUBLE_EQ(23.0 * cOneMeterLatitude.value(),
            ph.getConcisePoints().front().latitude.value());

    pp.latitude += 10.0 * cOneMeterLatitude;
    ph.addSample(pp);
    EXPECT_EQ(3, ph.getConcisePoints().size());

    pp.latitude += 3.0 * cOneMeterLatitude;
    ph.addSample(pp);
    EXPECT_EQ(4, ph.getConcisePoints().size());
    EXPECT_DOUBLE_EQ(43.0 * cOneMeterLatitude.value(),
            ph.getConcisePoints().front().latitude.value());

    pp.latitude += 23.0 * cOneMeterLatitude;
    ph.addSample(pp);
    EXPECT_EQ(5, ph.getConcisePoints().size());
    EXPECT_DOUBLE_EQ(46.0 * cOneMeterLatitude.value(),
            ph.getConcisePoints().front().latitude.value());
}

TEST(PathHistory, concise_points_actual_error_threshold) {
    PathHistory ph;
    PathPoint pp;
    ph.addSample(pp);

    pp.heading += units::Angle(5.0 * units::degrees);
    ph.addSample(pp);
    pp.latitude += 5.0 * cOneMeterLatitude;
    ph.addSample(pp);
    EXPECT_EQ(1, ph.getConcisePoints().size());

    pp.latitude += 10.0 * units::degrees;
    pp.longitude += 10.0 * units::degrees;
    ph.addSample(pp);
    EXPECT_EQ(2, ph.getConcisePoints().size());
}

TEST(PathHistory, concise_points_truncation) {
    PathHistory ph;
    PathPoint pp;
    pp.latitude = 0.0 * units::degree;
    pp.longitude = 0.0 * units::degree;
    ph.addSample(pp);

    pp.latitude += 25.0 * cOneMeterLatitude;
    ph.addSample(pp);
    pp.latitude += 25.0 * cOneMeterLatitude;
    ph.addSample(pp);
    EXPECT_EQ(2, ph.getConcisePoints().size());

    pp.latitude += 25.0 * cOneMeterLatitude;
    ph.addSample(pp);
    EXPECT_EQ(3, ph.getConcisePoints().size());

    pp.latitude += 205.0 * cOneMeterLatitude;
    ph.addSample(pp);
    ASSERT_EQ(4, ph.getConcisePoints().size());
    EXPECT_DOUBLE_EQ(0.0 * cOneMeterLatitude.value(),
            ph.getConcisePoints().back().latitude.value());
    EXPECT_DOUBLE_EQ(75.0 * cOneMeterLatitude.value(),
            ph.getConcisePoints().front().latitude.value());

    ph.addSample(pp);
    ASSERT_EQ(2, ph.getConcisePoints().size());
    EXPECT_DOUBLE_EQ(75.0 * cOneMeterLatitude.value(),
            ph.getConcisePoints().back().latitude.value());
    EXPECT_DOUBLE_EQ(280.0 * cOneMeterLatitude.value(),
            ph.getConcisePoints().front().latitude.value());
}
