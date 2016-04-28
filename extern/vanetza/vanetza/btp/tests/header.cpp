#include <gtest/gtest.h>
#include <vanetza/btp/header.hpp>
#include <vanetza/common/serialization_buffer.hpp>

using namespace vanetza;

const ByteBuffer ref = { 0x38, 0x84, 0x01, 0x40 };

TEST(BtpHeaderB, serialize) {
    btp::HeaderB header;
    header.destination_port = host_cast<uint16_t>(0x3884);
    header.destination_port_info = host_cast<uint16_t>(0x0140);
    ByteBuffer buf;
    serialize_into_buffer(header, buf);
    EXPECT_EQ(ref, buf);
}

TEST(BtpHeaderB, deserialize) {
    btp::HeaderB header;
    deserialize_from_buffer(header, ref);
    EXPECT_EQ(host_cast<uint16_t>(0x3884), header.destination_port);
    EXPECT_EQ(host_cast<uint16_t>(0x0140), header.destination_port_info);
}

