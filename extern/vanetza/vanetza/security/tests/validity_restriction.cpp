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

    Duration dur(b, Duration::Units::Years);
    Duration dur2(a);

    EXPECT_EQ(dur.raw(), a);
    EXPECT_EQ(dur2.raw(), a);
}

TEST(ValidityRestriction, Serialization)
{
    std::list<ValidityRestriction> list;
    EndValidity ev = 0x548736;
    list.push_back(ev);
    StartAndEndValidity sev;
    sev.start_validity = 0x54;
    sev.end_validity = 0x5712;
    list.push_back(sev);
    StartAndDurationValidity sdv;
    sdv.start_validity = 0x5712;
    sdv.duration = Duration { 13, Duration::Units::Hours };
    list.push_back(sdv);
    GeographicRegion grv = CircularRegion {};
    list.push_back(grv);

    check(list, serialize_roundtrip(list));
}

TEST(ValidityRestriction, WebValidator_Size)
{
    std::list<ValidityRestriction> list;
    ValidityRestriction res;
    StartAndEndValidity start;
    start.start_validity = 12345;
    start.end_validity = 4786283;
    res = start;
    list.push_back(res);

    GeographicRegion reg;
    IdentifiedRegion id;
    id.region_dictionary = RegionDictionary::Un_Stats;
    id.region_identifier = 150;
    id.local_region.set(0);
    reg = id;
    res = reg;
    list.push_back(res);

    EXPECT_EQ(15, get_size(list));
}
