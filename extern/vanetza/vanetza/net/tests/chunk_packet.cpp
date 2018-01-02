#include <gtest/gtest.h>
#include <vanetza/net/chunk_packet.hpp>

using vanetza::ByteBuffer;
using vanetza::ChunkPacket;
using vanetza::OsiLayer;

TEST(ChunkPacket, ctor)
{
    ChunkPacket packet;
    EXPECT_EQ(0, packet.size());
}

TEST(ChunkPacket, size_from_to)
{
    ChunkPacket packet;
    packet[OsiLayer::Link] = ByteBuffer(8);
    packet[OsiLayer::Session] = ByteBuffer(19);
    packet[OsiLayer::Application] = ByteBuffer(5);

    EXPECT_EQ(8, packet.size(OsiLayer::Link, OsiLayer::Link));
    EXPECT_EQ(0, packet.size(OsiLayer::Network, OsiLayer::Transport));
    EXPECT_EQ(27, packet.size(OsiLayer::Physical, OsiLayer::Session));
    EXPECT_EQ(24, packet.size(OsiLayer::Session, OsiLayer::Application));
}

TEST(ChunkPacket, access)
{
    const ByteBuffer data { 3, 8, 7, 5, 6 };
    ChunkPacket packet;
    packet[OsiLayer::Transport] = ByteBuffer { data };

    EXPECT_EQ(data.size(), packet.size());
    EXPECT_EQ(data.size(), packet[OsiLayer::Transport].size());

    ByteBuffer tmp;
    packet[OsiLayer::Transport].convert(tmp);
    EXPECT_EQ(data, tmp);
}

TEST(ChunkPacket, copy)
{
    ChunkPacket original;
    original[OsiLayer::Physical] = ByteBuffer(12);
    original[OsiLayer::Presentation] = ByteBuffer(34);

    ChunkPacket copy { original };
    EXPECT_EQ(46, copy.size());
    EXPECT_EQ(12, copy[OsiLayer::Physical].size());
    EXPECT_EQ(34, copy[OsiLayer::Presentation].size());

    ChunkPacket tmp;
    tmp = original;
    EXPECT_EQ(46, tmp.size());
}

TEST(ChunkPacket, self_assignment)
{
    ChunkPacket data;
    data[OsiLayer::Physical] = ByteBuffer(20);
    ASSERT_EQ(20, data.size());
    data = data;
    EXPECT_EQ(20, data.size());
}

TEST(ChunkPacket, extract)
{
    std::array<ByteBuffer, 5> buffer;
    for (unsigned i = 0; i < buffer.size(); ++i) {
        ByteBuffer tmp;
        tmp.resize(10, static_cast<ByteBuffer::value_type>(i));
        buffer[i] = std::move(tmp);
    }

    ChunkPacket a;
    a[OsiLayer::Link] = ByteBuffer { buffer[0] };
    a[OsiLayer::Network] = ByteBuffer { buffer[1] };
    a[OsiLayer::Transport] = ByteBuffer { buffer[2] };
    a[OsiLayer::Session] = ByteBuffer { buffer[3] };
    a[OsiLayer::Application] = ByteBuffer { buffer[4] };
    EXPECT_EQ(50, a.size());

    ChunkPacket b = a.extract(OsiLayer::Network, OsiLayer::Presentation);
    EXPECT_EQ(20, a.size());
    EXPECT_EQ(30, b.size());

    ByteBuffer tmp;
    a[OsiLayer::Link].convert(tmp);
    EXPECT_EQ(buffer[0], tmp);
    a[OsiLayer::Application].convert(tmp);
    EXPECT_EQ(buffer[4], tmp);

    b[OsiLayer::Network].convert(tmp);
    EXPECT_EQ(buffer[1], tmp);
    b[OsiLayer::Transport].convert(tmp);
    EXPECT_EQ(buffer[2], tmp);
    b[OsiLayer::Session].convert(tmp);
    EXPECT_EQ(buffer[3], tmp);
}

TEST(ChunkPacket, merge)
{
    std::array<ByteBuffer, 11> buffer;
    for (unsigned i = 0; i < buffer.size(); ++i) {
        ByteBuffer tmp;
        tmp.resize(10, static_cast<ByteBuffer::value_type>(i));
        buffer[i] = std::move(tmp);
    }

    ChunkPacket a;
    a[OsiLayer::Physical] = ByteBuffer { buffer[0] };
    a[OsiLayer::Link] = ByteBuffer { buffer[1] };
    a[OsiLayer::Network] = ByteBuffer { buffer[2] };
    a[OsiLayer::Transport] = ByteBuffer { buffer[3] };
    a[OsiLayer::Session] = ByteBuffer { buffer[4] };
    a[OsiLayer::Application] = ByteBuffer { buffer[5] };
    ASSERT_EQ(60, a.size());

    ChunkPacket b;
    b[OsiLayer::Physical] = ByteBuffer { buffer[6] };
    b[OsiLayer::Link] = ByteBuffer { buffer[7] };
    b[OsiLayer::Network] = ByteBuffer { buffer[8] };
    b[OsiLayer::Transport] = ByteBuffer { buffer[9] };
    b[OsiLayer::Application] = ByteBuffer { buffer[10] };
    ASSERT_EQ(50, b.size());

    a.merge(b, OsiLayer::Link, OsiLayer::Presentation);
    EXPECT_EQ(50, a.size());

    ByteBuffer tmp;
    a[OsiLayer::Physical].convert(tmp);
    EXPECT_EQ(buffer[0], tmp);
    a[OsiLayer::Link].convert(tmp);
    EXPECT_EQ(buffer[7], tmp);
    a[OsiLayer::Network].convert(tmp);
    EXPECT_EQ(buffer[8], tmp);
    a[OsiLayer::Transport].convert(tmp);
    EXPECT_EQ(buffer[9], tmp);
    a[OsiLayer::Session].convert(tmp);
    EXPECT_TRUE(tmp.empty());
    a[OsiLayer::Presentation].convert(tmp);
    EXPECT_TRUE(tmp.empty());
    a[OsiLayer::Application].convert(tmp);
    EXPECT_EQ(buffer[5], tmp);
}
