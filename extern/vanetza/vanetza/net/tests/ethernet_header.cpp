#include <gtest/gtest.h>
#include <vanetza/common/byte_order.hpp>
#include <vanetza/net/ethernet_header.hpp>
#include <vanetza/net/mac_address.hpp>

using namespace vanetza;

TEST(EthernetHeader, create) {
    MacAddress dest = { 0x12, 0x34, 0x01, 0x47, 0x43, 0x43 };
    MacAddress src = { 0xfa, 0xbc, 0x8d, 0x43, 0xcb, 0x00 };

    ByteBuffer header = create_ethernet_header(dest, src, host_cast<uint16_t>(0x3773));
    ByteBuffer expected = { 0x12, 0x34, 0x01, 0x47, 0x43, 0x43, 0xfa, 0xbc, 0x8d, 0x43, 0xcb, 0x00, 0x37, 0x73 };
    EXPECT_EQ(expected, header);
}

TEST(EthernetHeader, length) {
    // magic constants are okay in unit tests
    EXPECT_EQ(14, ethernet_header_length());
}

