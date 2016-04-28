#include <gtest/gtest.h>
#include <vanetza/security/payload.hpp>
#include <vanetza/security/tests/check_payload.hpp>
#include <vanetza/security/tests/serialization.hpp>

using namespace vanetza;
using namespace vanetza::security;

TEST(Payload, serialize_cohesive)
{
    Payload p;
    p.type = PayloadType::Unsecured;
    p.data = CohesivePacket({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 }, OsiLayer::Application);

    check(p, serialize_roundtrip(p));
}

TEST(Payload, serialize_chunk)
{
    Payload p;
    p.type = PayloadType::Encrypted;
    ChunkPacket packet;
    packet[OsiLayer::Network] = ByteBuffer { 1, 2, 3, 4 };
    packet[OsiLayer::Transport] = ByteBuffer { 5, 6, 7, 8 };
    packet[OsiLayer::Application] = ByteBuffer { 9, 10, 11, 12 };
    p.data = packet;

    check(p, serialize_roundtrip(p));
}

TEST(Payload, size)
{
    Payload p;
    p.type = PayloadType::Signed;
    p.data = CohesivePacket({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, OsiLayer::Session);
    EXPECT_EQ(1 /* type */ + 1 /* length coding */ + 10 /* bytes */, get_size(p));
}

