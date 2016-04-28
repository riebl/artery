#include <gtest/gtest.h>
#include <vanetza/security/region.hpp>
#include <vanetza/security/tests/check_region.hpp>
#include <vanetza/security/tests/serialization.hpp>
#include <vanetza/units/angle.hpp>
#include <vanetza/units/length.hpp>

using namespace vanetza::security;
using vanetza::geonet::distance_u16t;
using vanetza::geonet::geo_angle_i32t;
using vanetza::units::degrees;
using vanetza::units::si::meter;

TEST(Region, Serialize_CircularRegion)
{
    CircularRegion reg;
    reg.center.latitude = static_cast<geo_angle_i32t>(12564 * degrees);
    reg.center.longitude = static_cast<geo_angle_i32t>(654321 * degrees);
    reg.radius = static_cast<distance_u16t>(1337 * meter);
    check(reg, serialize_roundtrip(reg));
}

TEST(Region, Serialize_IdentifiedRegion)
{
    IdentifiedRegion reg;
    reg.region_dictionary = RegionDictionary::Iso_3166_1;
    reg.region_identifier = 12345;
    reg.local_region.set(546);
    check(reg, serialize_roundtrip(reg));
}

TEST(Region, Serialize_PolygonalRegion)
{
    PolygonalRegion reg;
    for (std::size_t i = 0; i < 3; ++i) {
        reg.push_back(TwoDLocation {
                geo_angle_i32t::from_value(25 + i),
                geo_angle_i32t::from_value(26 + i)
            });
    }
    check(reg, serialize_roundtrip(reg));
}

TEST(Region, Serialize_RectangularRegion_list)
{
    std::list<RectangularRegion> reg;
    for (std::size_t i = 0; i < 5; ++i) {
        reg.push_back(RectangularRegion {
            TwoDLocation {
                geo_angle_i32t::from_value(1000000 + i),
                geo_angle_i32t::from_value(1010000 + i)
            },
            TwoDLocation {
                geo_angle_i32t::from_value(1020000 + i),
                geo_angle_i32t::from_value(1030000 + i)
            }
        });
    }
    check(reg, serialize_roundtrip(reg));
}
