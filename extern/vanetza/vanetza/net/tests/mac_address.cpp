#include <gtest/gtest.h>
#include <vanetza/net/mac_address.hpp>
#include <sstream>

using namespace vanetza;

TEST(MacAddress, ctor) {
    MacAddress empty;
    EXPECT_EQ(empty.octets.size(), 6);
    for (uint8_t octet : empty.octets) {
        EXPECT_EQ(octet, 0x00);
    }

    MacAddress init = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
    EXPECT_EQ(init.octets[0], 0x01);
    EXPECT_EQ(init.octets[1], 0x02);
    EXPECT_EQ(init.octets[2], 0x03);
    EXPECT_EQ(init.octets[3], 0x04);
    EXPECT_EQ(init.octets[4], 0x05);
    EXPECT_EQ(init.octets[5], 0x06);
}

TEST(MacAddress, equality) {
    MacAddress a = { 1, 2, 3, 4, 5, 6 };
    MacAddress b = { 1, 2, 3, 4, 5, 7 };
    MacAddress c = { 0, 2, 3, 4, 5, 6 };
    MacAddress d = { 0, 2, 3, 4, 5, 6 };
    EXPECT_NE(a, b);
    EXPECT_NE(b, a);
    EXPECT_NE(a, c);
    EXPECT_NE(b, c);
    EXPECT_EQ(a, a);
    EXPECT_EQ(c, d);
}

TEST(MacAddress, less) {
    MacAddress a = { 1, 2, 3, 4, 5, 6};
    MacAddress b = { 1, 2, 4, 4, 5, 6};
    MacAddress c = { 250, 0, 0, 0, 0, 0};
    MacAddress d = { 0, 2, 3, 4, 5, 6};
    MacAddress e = { 1, 2, 3, 4, 5, 5};
    EXPECT_LT(a, b);
    EXPECT_LT(a, b);
    EXPECT_LT(d, e);
    EXPECT_LT(e, a);
    EXPECT_LT(c, d);
    EXPECT_GT(b, a);
}

TEST(MacAddress, ostream) {
    MacAddress mac = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
    std::stringstream sout;
    sout << mac;
    EXPECT_EQ(sout.str(), "01:02:03:04:05:06");
}

TEST(MacAddress, parse) {
    MacAddress result;

    ASSERT_TRUE(!!parse_mac_address("01:02:03:04:05:06", result));
    EXPECT_EQ(MacAddress({1, 2, 3, 4, 5, 6}), result);
    EXPECT_TRUE(!!parse_mac_address("01:02:03:04:05:06"));

    EXPECT_FALSE(!!parse_mac_address(":::::0102040506", result));
    EXPECT_FALSE(!!parse_mac_address("foobarfoobarfoob", result));
    EXPECT_FALSE(!!parse_mac_address("01:02:03:04:05", result));
    EXPECT_FALSE(!!parse_mac_address("01:02:03:04:05:06:07", result));
}

TEST(MacAddress, create) {
    MacAddress a = create_mac_address(static_cast<uint32_t>(0x12345678));
    EXPECT_EQ(a, MacAddress({0x12, 0x34, 0x56, 0x78, 0x00, 0x00}));

    MacAddress b = create_mac_address(static_cast<uint64_t>(0x1234567890abcdef));
    EXPECT_EQ(b, MacAddress({0x56, 0x78, 0x90, 0xab, 0xcd, 0xef}));

    MacAddress c = create_mac_address(static_cast<uint32_t>(0x1234));
    EXPECT_EQ(c, MacAddress({0x00, 0x00, 0x12, 0x34, 0x00, 0x00}));

    MacAddress d = create_mac_address(static_cast<uint16_t>(0x2156));
    EXPECT_EQ(d, MacAddress({0x21, 0x56, 0x00, 0x00, 0x00, 0x00}));
}

TEST(MacAddress, hash) {
    std::hash<MacAddress> hash_fn;

    MacAddress a, b;
    EXPECT_EQ(hash_fn(a), hash_fn(b));

    MacAddress c = { 0x01, 0x00, 0x00, 0x00, 0x00, 0x00 };
    EXPECT_NE(hash_fn(a), hash_fn(c));

    MacAddress d = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };
    EXPECT_NE(hash_fn(a), hash_fn(d));
}

