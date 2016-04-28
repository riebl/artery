#include <gtest/gtest.h>
#include <vanetza/security/tests/check_region.hpp>
#include <vanetza/security/tests/check_visitor.hpp>
#include <boost/format.hpp>

namespace vanetza
{
namespace security
{

void check(const ThreeDLocation& expected, const ThreeDLocation& actual)
{
    EXPECT_EQ(expected.latitude, actual.latitude);
    EXPECT_EQ(expected.longitude, actual.longitude);
    EXPECT_EQ(expected.elevation, actual.elevation);
}

void check(const TwoDLocation& expected, const TwoDLocation& actual)
{
    EXPECT_EQ(expected.longitude, actual.longitude);
    EXPECT_EQ(expected.latitude, actual.latitude);
}

void check(const CircularRegion& expected, const CircularRegion& actual)
{
    SCOPED_TRACE("CiruclarRegion");
    check(expected.center, actual.center);
    EXPECT_EQ(expected.radius, actual.radius);
}

void check(const RectangularRegion& expected, const RectangularRegion& actual)
{
    SCOPED_TRACE("RectangularRegion");
    check(expected.northwest, actual.northwest);
    check(expected.southeast, actual.southeast);
}

void check(std::list<RectangularRegion> expected, std::list<RectangularRegion> actual)
{
    SCOPED_TRACE("list<RectangularRegion>");
    ASSERT_EQ(expected.size(), actual.size());
    for (std::size_t i = 0, j = expected.size(); i < j; ++i) {
        SCOPED_TRACE(boost::format("Rectangle #%1%") % i);
        check(expected.front(), actual.front());
        expected.pop_front();
        actual.pop_front();
    }
}

void check(PolygonalRegion expected, PolygonalRegion actual)
{
    SCOPED_TRACE("PolygonalRegion");
    ASSERT_EQ(expected.size(), actual.size());
    for (std::size_t i = 0, j = expected.size(); i < j; ++i) {
        SCOPED_TRACE(boost::format("Coordinate #%1%") % i);
        check(expected.front(), actual.front());
        expected.pop_front();
        actual.pop_front();
    }
}

void check(const IdentifiedRegion& expected, const IdentifiedRegion& actual)
{
    EXPECT_EQ(expected.region_dictionary, actual.region_dictionary);
    EXPECT_EQ(expected.region_identifier, actual.region_identifier);
    EXPECT_EQ(expected.local_region, actual.local_region);
}

void check(const GeographicRegion& expected, const GeographicRegion& actual)
{
    ASSERT_EQ(get_type(expected), get_type(actual));
    boost::apply_visitor(check_visitor<GeographicRegion>(), expected, actual);
}

} // namespace security
} // namespace vanetza
