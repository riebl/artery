#include <gtest/gtest.h>
#include <vanetza/geonet/address.hpp>
#include <vanetza/geonet/serialization_buffer.hpp>
#include <vanetza/net/mac_address.hpp>

using namespace vanetza;
using namespace vanetza::geonet;

TEST(Address, ctor) {
    Address a;
    EXPECT_FALSE(a.is_manually_configured());
    EXPECT_EQ(a.mid(), MacAddress());
    EXPECT_EQ(a.station_type(), StationType::UNKNOWN);
    EXPECT_EQ(a.country_code(), 0);
}

TEST(Address, equality) {
    Address a;
    Address b({0x01, 0x02, 0x03, 0x04, 0x05, 0x06});
    EXPECT_NE(a, b);
    Address c = b;
    EXPECT_EQ(b, c);
    c.is_manually_configured(true);
    EXPECT_NE(b, c);
    Address d = c;
    EXPECT_EQ(c, d);
    d.station_type(StationType::PASSENGER_CAR);
    EXPECT_NE(c, d);
    Address e = d;
    EXPECT_EQ(d, e);
    e.country_code(8);
    EXPECT_NE(d, e);
    a.mid(b.mid());
    EXPECT_EQ(a, b);
}

TEST(Address, serialization) {
    Address a({1, 2, 3, 4, 5, 6});
    a.is_manually_configured(true);
    a.station_type(StationType::TRAM);
    a.country_code(0x0333);

    ByteBuffer buffer;
    serialize_into_buffer(a, buffer);
    EXPECT_EQ(Address::length_bytes, buffer.size());

    Address b;
    deserialize_from_buffer(b, buffer);
    EXPECT_EQ(a, b);
}

