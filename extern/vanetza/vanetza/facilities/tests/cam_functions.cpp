#include <gtest/gtest.h>
#include <vanetza/facilities/cam_functions.hpp>
#include <boost/units/cmath.hpp>
#include <boost/units/io.hpp>
#include <cmath>

using namespace vanetza;
using namespace vanetza::facilities;
using namespace vanetza::units;

constexpr double microdegree(double degree, double min)
{
    return 1000.0 * 1000.0 * (degree + min / 60.0);
}

::testing::AssertionResult NearDistance(const Length& a, const Length& b, Length delta)
{
    using namespace boost::units;
    const auto diff = abs(a - b);
    if (diff < delta) {
        return ::testing::AssertionSuccess();
    } else {
        return ::testing::AssertionFailure() << "actual difference " << diff << " exceeds delta of " << delta;
    }
}

TEST(CamFunctions, similar_heading)
{
    Angle a = 3 * si::radian;
    Angle b = 2 * si::radian;
    Angle limit = 0.5 * si::radian;
    EXPECT_FALSE(similar_heading(a, b, limit));
    EXPECT_FALSE(similar_heading(b, a, limit));

    limit = 1.0 * si::radian;
    EXPECT_TRUE(similar_heading(a, b, limit));
    EXPECT_TRUE(similar_heading(b, a, limit));

    a = 6.1 * si::radian;
    b = 0.2 * si::radian;
    limit = 0.4 * si::radian;
    EXPECT_TRUE(similar_heading(a, b, limit));
    EXPECT_TRUE(similar_heading(b, a, limit));

    limit = 0.3 * si::radian;
    EXPECT_FALSE(similar_heading(a, b, limit));
    EXPECT_FALSE(similar_heading(b, a, limit));

    limit = -1.0 * si::radian;
    EXPECT_FALSE(similar_heading(a, a, limit));
}

TEST(CamFunctions, similar_heading_unavailable1)
{
    Heading a;
    a.headingValue = HeadingValue_unavailable;
    Angle b = 2.0 * si::radian;
    Angle limit = 10 * si::radian;
    EXPECT_FALSE(is_available(a));
    EXPECT_FALSE(similar_heading(a, b, limit));

    a.headingValue = 2 * HeadingValue_wgs84East;
    EXPECT_TRUE(is_available(a));
    EXPECT_TRUE(similar_heading(a, b, limit));

    b = 0.0 * si::radian;
    limit = 3.14 * si::radian;
    EXPECT_FALSE(similar_heading(a, b, limit));

    limit = 3.15 * si::radian;
    EXPECT_TRUE(similar_heading(a, b, limit));
}

TEST(CamFunctions, similar_heading_unavailable2)
{
    Heading a;
    a.headingValue = HeadingValue_unavailable;
    Heading b;
    b.headingValue = HeadingValue_unavailable;
    Angle limit = 10 * si::radian;
    EXPECT_FALSE(is_available(a));
    EXPECT_FALSE(is_available(b));
    EXPECT_FALSE(similar_heading(a, b, limit));

    b.headingValue = 200;
    EXPECT_TRUE(is_available(b));
    EXPECT_FALSE(similar_heading(a, b, limit));
    EXPECT_FALSE(similar_heading(b, a, limit));

    a.headingValue = 300;
    EXPECT_TRUE(is_available(a));
    EXPECT_TRUE(similar_heading(a, b, limit));
    EXPECT_TRUE(similar_heading(b, a, limit));
}

TEST(CamFunctions, distance_reference_positions)
{
    ReferencePosition_t pos1;
    pos1.latitude = microdegree(6, 21.23) * Latitude_oneMicrodegreeSouth;
    pos1.longitude = microdegree(33, 22.12) * Longitude_oneMicrodegreeWest;
    ReferencePosition_t pos2;
    pos2.latitude = microdegree(6, 22.48) * Latitude_oneMicrodegreeSouth;
    pos2.longitude = microdegree(33, 22.55) * Longitude_oneMicrodegreeWest;
    EXPECT_TRUE(NearDistance(distance(pos1, pos2), 2440.0 * si::meter , 10.0 * si::meter));

    ReferencePosition_t pos3;
    pos3.latitude = microdegree(37, 17.3) * Latitude_oneMicrodegreeNorth;
    pos3.longitude = microdegree(0, 13.14) * Longitude_oneMicrodegreeWest;
    ReferencePosition_t pos4;
    pos4.latitude = microdegree(37, 17.19) * Latitude_oneMicrodegreeNorth;
    pos4.longitude = microdegree(0, 9.45) * Longitude_oneMicrodegreeEast;
    EXPECT_TRUE(NearDistance(distance(pos3, pos4), 33390.0 * si::meter , 10.0 * si::meter));

    ReferencePosition_t pos5;
    pos5.latitude = microdegree(0, 19.24) * Latitude_oneMicrodegreeSouth;
    pos5.longitude = microdegree(83, 37.32) * Longitude_oneMicrodegreeEast;
    ReferencePosition_t pos6;
    pos6.latitude = microdegree(0, 27.15) * Latitude_oneMicrodegreeNorth;
    pos6.longitude = microdegree(83, 04.45) * Longitude_oneMicrodegreeEast;
    EXPECT_TRUE(NearDistance(distance(pos5, pos6), 105010.0 * si::meter , 10.0 * si::meter));

    ReferencePosition_t pos7;
    pos7.latitude = microdegree(48, 45.56) * Latitude_oneMicrodegreeNorth;
    pos7.longitude = microdegree(11, 26.01) * Longitude_oneMicrodegreeEast;
    ReferencePosition_t pos8;
    pos8.latitude = microdegree(48, 45.566) * Latitude_oneMicrodegreeNorth;
    pos8.longitude = microdegree(11, 26.04) * Longitude_oneMicrodegreeEast;
    EXPECT_TRUE(NearDistance(distance(pos7, pos8), 38.0 * si::meter , 0.5 * si::meter));
}

TEST(CamFunctions, distance_refpos_latlon)
{
    ReferencePosition_t refpos;
    refpos.latitude = microdegree(6, 21.23) * Latitude_oneMicrodegreeSouth;
    refpos.longitude = microdegree(33, 22.12) * Longitude_oneMicrodegreeWest;
    GeoAngle lat = -(6 + (22.48 / 60.0)) * degree;
    GeoAngle lon = -(33 + (22.55 / 60.0)) * degree;
    EXPECT_TRUE(NearDistance(distance(refpos, lat, lon), 2440.0 * si::meter , 10.0 * si::meter));
}

TEST(CamFunctions, distance_unavailable)
{
    ReferencePosition_t pos1 {};
    ReferencePosition_t pos2 {};
    EXPECT_TRUE(is_available(pos1));
    EXPECT_TRUE(is_available(pos2));
    EXPECT_FALSE(std::isnan(distance(pos1, pos2).value()));

    pos1.latitude = Latitude_unavailable;
    EXPECT_FALSE(is_available(pos1));
    EXPECT_TRUE(std::isnan(distance(pos1, pos2).value()));
    EXPECT_TRUE(std::isnan(distance(pos2, pos1).value()));

    pos1.latitude = 0;
    pos1.longitude = Longitude_unavailable;
    EXPECT_FALSE(is_available(pos1));
    EXPECT_TRUE(std::isnan(distance(pos1, pos2).value()));
    EXPECT_TRUE(std::isnan(distance(pos2, pos1).value()));
}
