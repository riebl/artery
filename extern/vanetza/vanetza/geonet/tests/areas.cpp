#include <gtest/gtest.h>
#include <vanetza/geonet/areas.hpp>
#include <vanetza/units/length.hpp>

using namespace vanetza::geonet;
namespace units = vanetza::units;
using units::si::meter;
using units::degree;
using units::si::square_meter;

TEST(Areas, cartesian_substraction) {
    CartesianPosition a(-3.4 * meter , 8.3 * meter);
    CartesianPosition b(34.8 * meter, -14.8 * meter);
    CartesianPosition c = a - b;
    EXPECT_DOUBLE_EQ(c.x / meter, -38.2);
    EXPECT_DOUBLE_EQ(c.y / meter, 23.1);
}

TEST(Areas, geodetic_distance) {
    GeodeticPosition a(48.76714 * degree, 11.43263 * degree);
    GeodeticPosition b(-25.41272 * degree, -49.24815 * degree);
    units::Length d = distance(a, b);
    EXPECT_NEAR(d / meter, 10185367.442, 0.0005);
}

TEST(Areas, geometric_function_circle) {
    Circle c;
    c.r = 38.4 * meter;
    CartesianPosition p(0.0 * meter, 0.0 * meter);

    EXPECT_TRUE(at_center_point(c, p));
    EXPECT_TRUE(inside_shape(c, p));
    EXPECT_FALSE(outside_shape(c, p));
    EXPECT_FALSE(at_shape_border(c, p));

    p.x = 15.0 * meter;
    p.y = 36.0 * meter;
    EXPECT_TRUE(outside_shape(c, p));
}

TEST(Areas, geometric_function_rectangle) {
    Rectangle r;
    r.a = 8.5 * meter;
    r.b = 3.0 * meter;
    CartesianPosition p(-3.5 * meter, 2.9 * meter);
    EXPECT_TRUE(inside_shape(r, p));

    p.x = -8.6 * meter;
    EXPECT_TRUE(outside_shape(r, p));
}

TEST(Areas, geometric_function_ellipse) {
    Ellipse e;
    e.a = 8.4 * meter;
    e.b = 6.5 * meter;
    CartesianPosition p(-7.6 * meter, 1.3 * meter);
    EXPECT_TRUE(inside_shape(e, p));

    p.y = -4.5 * meter;
    EXPECT_TRUE(outside_shape(e, p));
}

TEST(Areas, local_cartesian) {
    GeodeticPosition origin(48.76714 * degree, 11.43263 * degree); // THI
    GeodeticPosition datum(48.7656 * degree, 11.4296 * degree); // ZAF
    CartesianPosition pos = local_cartesian(origin, datum);
    EXPECT_NEAR(pos.x / meter, -222.74, 0.01);
    EXPECT_NEAR(pos.y / meter, -171.25, 0.01);
}

TEST(Areas, canonicalize) {
    CartesianPosition point(9.0 * meter, 5.0 * meter);
    CartesianPosition shape_center(6.0 * meter, 3.0 * meter);
    units::Angle azimuth = units::Angle(30.0 * degree);
    CartesianPosition canonical_point = canonicalize(point, shape_center, azimuth);
    EXPECT_NEAR(canonical_point.x / meter, 3.23, 0.01);
    EXPECT_NEAR(canonical_point.y / meter, -1.59, 0.01);
}

TEST(Areas, inside_or_at_border) {
    Rectangle r;
    r.a = 300.0 * meter;
    r.b = 170.0 * meter;
    Area a;
    a.shape = r;
    a.angle = units::Angle(90.0 * degree);
    a.position = GeodeticPosition(48.7656 * degree, 11.4296 * degree);
    GeodeticPosition ego(48.76714 * degree, 11.43263 * degree);
    EXPECT_FALSE(inside_or_at_border(a, ego));
    a.angle = units::Angle(45.0 * degree);
    EXPECT_TRUE(inside_or_at_border(a, ego));
}

TEST(Areas, area_size) {
    Circle c;
    c.r = 18.3 * meter;
    Rectangle r;
    r.a = 393.0 * meter;
    r.b = 140.8 * meter;
    Ellipse e;
    e.a = 393.0 * meter;
    e.b = 140.8 * meter;

    Area a;
    a.shape = c;
    EXPECT_NEAR(area_size(a) / square_meter, 1052.0880, 0.0001);
    a.shape = r;
    EXPECT_NEAR(area_size(a) / square_meter, 221337.6000, 0.0001);
    a.shape = e;
    EXPECT_NEAR(area_size(a) / square_meter, 173838.1445, 0.0001);
}

