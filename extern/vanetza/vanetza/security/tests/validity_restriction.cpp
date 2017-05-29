#include <gtest/gtest.h>
#include <vanetza/common/bit_number.hpp>
#include <vanetza/security/validity_restriction.hpp>
#include <vanetza/security/tests/check_validity_restriction.hpp>
#include <vanetza/security/tests/serialization.hpp>

using namespace vanetza::security;

TEST(Duration, Duration)
{
    uint16_t a = 0x8007;
    uint16_t b = 7;

    Duration dur1(b, Duration::Units::Years);
    Duration dur2(a);

    EXPECT_EQ(dur1.raw(), a);
    EXPECT_EQ(dur2.raw(), a);
}

TEST(ValidityRestriction, Serialization)
{
    std::list<ValidityRestriction> list;
    list.push_back(EndValidity { 0x548736 });
    list.push_back(StartAndEndValidity { 0x54, 0x5712});
    list.push_back(StartAndDurationValidity { 0x5712, Duration(13, Duration::Units::Hours)});
    list.push_back(GeographicRegion { CircularRegion {} });

    check(list, serialize_roundtrip(list));
}

TEST(ValidityRestriction, WebValidator_Size)
{
    std::list<ValidityRestriction> list;
    StartAndEndValidity start;
    start.start_validity = 12345;
    start.end_validity = 4786283;
    list.push_back(start);

    IdentifiedRegion id;
    id.region_dictionary = RegionDictionary::Un_Stats;
    id.region_identifier = 150;
    id.local_region.set(0);
    list.push_back(GeographicRegion { id });

    EXPECT_EQ(15, get_size(list));
    // TODO: compare raw bytes
}
