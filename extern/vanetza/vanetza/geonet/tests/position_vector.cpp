#include <gtest/gtest.h>
#include <vanetza/geonet/position_vector.hpp>
#include <vanetza/geonet/serialization_buffer.hpp>

using vanetza::ByteBuffer;
using vanetza::MacAddress;
using namespace vanetza::geonet;

TEST(PositionVector, lpv_serialization)
{
    LongPositionVector lpv1;
    lpv1.gn_addr.is_manually_configured(true);
    lpv1.gn_addr.station_type(StationType::MOTORCYCLE);
    lpv1.gn_addr.country_code(0x0333);
    lpv1.gn_addr.mid(MacAddress { 1, 2, 3, 0xa, 0xb, 0xc });
    lpv1.timestamp += 4321 * Timestamp::millisecond();
    lpv1.latitude.from_value(0x1234);
    lpv1.longitude.from_value(0xabcd);
    lpv1.position_accuracy_indicator = false;
    lpv1.speed.from_value(12345);
    lpv1.heading.from_value(0xef);

    ByteBuffer buffer;
    serialize_into_buffer(lpv1, buffer);
    EXPECT_EQ(LongPositionVector::length_bytes, buffer.size());

    LongPositionVector lpv2;
    deserialize_from_buffer(lpv2, buffer);
    EXPECT_EQ(lpv1.gn_addr, lpv2.gn_addr);
    EXPECT_EQ(lpv1, lpv2);
}

TEST(PositionVector, zero_initialized)
{
    // "At start-up, all data elements of the LPV shall be initialized
    // with 0 to indicate an unknown value." (EN 302 636-4-1 V1.2.0)
    LongPositionVector lpv;
    EXPECT_TRUE(is_empty(lpv));

    ByteBuffer buffer;
    serialize_into_buffer(lpv, buffer);
    ByteBuffer zero;
    zero.assign(LongPositionVector::length_bytes, 0x00);
    EXPECT_EQ(zero, buffer);
}
