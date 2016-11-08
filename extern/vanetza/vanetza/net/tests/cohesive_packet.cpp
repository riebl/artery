#include <gtest/gtest.h>
#include <vanetza/net/cohesive_packet.hpp>
#include <vanetza/common/byte_buffer.hpp>

using vanetza::ByteBuffer;
using vanetza::CohesivePacket;
using vanetza::OsiLayer;

TEST(CohesivePacket, ctor_and_size)
{
    const ByteBuffer buffer(892);
    CohesivePacket packet(buffer, OsiLayer::Transport);

    EXPECT_EQ(892, packet.size());
    EXPECT_EQ(892, packet.size(OsiLayer::Transport));
    EXPECT_EQ(0, packet.size(OsiLayer::Network));
    EXPECT_EQ(0, packet.size(OsiLayer::Session));
}

TEST(CohesivePacket, set_boundary)
{
    CohesivePacket packet(ByteBuffer(512), OsiLayer::Link);
    ASSERT_EQ(512, packet.size(OsiLayer::Link));

    packet.set_boundary(OsiLayer::Link, 64);
    EXPECT_EQ(64, packet.size(OsiLayer::Link));
    EXPECT_EQ(512, packet.size());

    ASSERT_EQ(448, packet.size(OsiLayer::Network));
    packet.set_boundary(OsiLayer::Network, 128);
    EXPECT_EQ(128, packet.size(OsiLayer::Network));

    ASSERT_EQ(320, packet.size(OsiLayer::Transport));
    packet.set_boundary(OsiLayer::Transport, 0);
    EXPECT_EQ(0, packet.size(OsiLayer::Transport));
    EXPECT_EQ(320, packet.size(OsiLayer::Session));
    EXPECT_EQ(512, packet.size());

    EXPECT_EQ(448, packet.size(OsiLayer::Network, OsiLayer::Application));
    EXPECT_EQ(0, packet.size(OsiLayer::Application, OsiLayer::Network));
}

TEST(CohesivePacket, access)
{
    ByteBuffer buffer;
    for (unsigned i = 0; i < 512; ++i) {
        buffer.push_back(i);
    }
    CohesivePacket packet(buffer, OsiLayer::Network);

    auto net_range = packet[OsiLayer::Network];
    ASSERT_EQ(512, net_range.size());

    packet.set_boundary(OsiLayer::Network, 128);
    auto transport_range = packet[OsiLayer::Transport];
    ASSERT_EQ(384, transport_range.size());

    ByteBuffer::value_type expected_byte = 128;
    for (auto byte : transport_range) {
        EXPECT_EQ(expected_byte, byte);
        ++expected_byte;
    }
}

TEST(CohesivePacket, trim_simple_boundaries)
{
    CohesivePacket packet(ByteBuffer(128), OsiLayer::Link);
    EXPECT_EQ(128, packet.size());
    packet.trim(OsiLayer::Physical, 100);
    EXPECT_EQ(100, packet.size());
    packet.trim(OsiLayer::Physical, 130);
    EXPECT_EQ(100, packet.size());
}

TEST(CohesivePacket, trim_advanced_boundaries)
{
    CohesivePacket packet(ByteBuffer(128), OsiLayer::Link);
    packet.set_boundary(OsiLayer::Link, 12);
    packet.set_boundary(OsiLayer::Network, 0);
    packet.set_boundary(OsiLayer::Transport, 24);
    packet.set_boundary(OsiLayer::Session, 48);
    EXPECT_EQ(128, packet.size(OsiLayer::Link, OsiLayer::Presentation));
    EXPECT_EQ(0, packet.size(OsiLayer::Physical));
    EXPECT_EQ(12, packet.size(OsiLayer::Link));
    EXPECT_EQ(0, packet.size(OsiLayer::Network));
    EXPECT_EQ(24, packet.size(OsiLayer::Transport));
    EXPECT_EQ(48, packet.size(OsiLayer::Session));
    EXPECT_EQ(44, packet.size(OsiLayer::Presentation));
    EXPECT_EQ(0, packet.size(OsiLayer::Application));

    packet.trim(OsiLayer::Network, 24 + 48 + 20);
    EXPECT_EQ(24 + 48 + 20, packet.size(OsiLayer::Network, OsiLayer::Application));
    EXPECT_EQ(104, packet.size());

    packet.trim(OsiLayer::Network, 24 + 8);
    EXPECT_EQ(44, packet.size());
    EXPECT_EQ(24, packet.size(OsiLayer::Transport));
    EXPECT_EQ(8, packet.size(OsiLayer::Session));
    EXPECT_EQ(0, packet.size(OsiLayer::Presentation, OsiLayer::Application));
}
