#include <gtest/gtest.h>
#include <vanetza/common/bit_number.hpp>
#include <vanetza/security/validity_restriction.hpp>
#include <vanetza/security/tests/check_validity_restriction.hpp>
#include <vanetza/security/tests/serialization.hpp>

using namespace vanetza::security;

TEST(Duration, Raw)
{
    uint16_t a = 0x8007;
    uint16_t b = 7;

    Duration dur1(b, Duration::Units::Years);
    Duration dur2(a);

    EXPECT_EQ(dur1.raw(), a);
    EXPECT_EQ(dur2.raw(), a);
}

TEST(Duration, Unit)
{
    const uint16_t raw = 0x8007;
    Duration duration(raw);

    EXPECT_EQ(duration.raw(), raw);
    EXPECT_EQ(duration.unit(), Duration::Units::Years);
}

TEST(Duration, Value)
{
    const uint16_t raw = 0x8007;
    Duration duration(raw);

    EXPECT_EQ(duration.raw(), raw);
    EXPECT_EQ(duration.value(), 7);
}

TEST(Duration, ToSeconds)
{
    const uint16_t raw = 0x2007;
    Duration duration(raw);

    EXPECT_EQ(duration.raw(), raw);
    EXPECT_EQ(duration.value(), 7);
    EXPECT_EQ(duration.to_seconds(), std::chrono::seconds(7 * 60));
}

TEST(Duration, ToSecondsMax)
{
    Duration duration(0xffff, Duration::Units::Years);
    EXPECT_EQ(duration.value(), 0x1fff);
    EXPECT_EQ(duration.unit(), Duration::Units::Years);
    EXPECT_EQ(duration.to_seconds(), std::chrono::seconds(static_cast<int64_t>(31556925) * 0x1fff));
}

TEST(Duration, ToSecondsInvalidUnit)
{
    Duration duration(0xffff);
    EXPECT_EQ(duration.to_seconds(), std::chrono::seconds::min());
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
