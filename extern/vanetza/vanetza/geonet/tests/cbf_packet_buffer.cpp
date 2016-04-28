#include <gtest/gtest.h>
#include <vanetza/common/runtime.hpp>
#include <vanetza/geonet/cbf_packet_buffer.hpp>
#include <vanetza/geonet/mib.hpp>
#include <functional>

using namespace vanetza;
using namespace vanetza::geonet;

static const size_t GbcPduLength = BasicHeader::length_bytes +
    CommonHeader::length_bytes + GeoBroadcastHeader::length_bytes;

std::size_t get_length(const CbfPacket::Data& data)
{
    return get_length(*data.pdu) + data.payload->size();
}

class CbfPacketBufferTest : public ::testing::Test
{
protected:
    using PduPtr = CbfPacket::PduPtr;

    void SetUp() override
    {
        runtime.reset(Clock::time_point { std::chrono::hours(42) });
        calls = 0;
        last_call_length = 0;
    }

    CbfPacket create_packet(const MacAddress&, SequenceNumber::value_type, std::size_t length = GbcPduLength) const;
    CbfPacket create_packet(std::size_t length = GbcPduLength) const;
    CbfPacketBuffer::TimerCallback callback();

    const MIB mib;
    Runtime runtime;
    unsigned calls;
    unsigned last_call_length;
};

CbfPacket CbfPacketBufferTest::create_packet(const MacAddress& mac, SequenceNumber::value_type sn, std::size_t size) const
{
    PduPtr pdu { new CbfPacketBufferTest::PduPtr::element_type(mib) };
    pdu->extended().source_position.gn_addr.mid(mac);
    pdu->extended().sequence_number = SequenceNumber { sn };
    CbfPacket::PayloadPtr payload { new CbfPacket::PayloadPtr::element_type() };
    assert(get_length(*pdu) <= size);
    const std::size_t payload_size = size - get_length(*pdu);
    payload->layer(OsiLayer::Application) = ByteBuffer(payload_size);
    return CbfPacket(std::move(pdu), std::move(payload));
}

CbfPacket CbfPacketBufferTest::create_packet(std::size_t length) const
{
    static unsigned counter = 0;
    return create_packet({0, 0, 0, 0, 0, 0}, ++counter, length);
}

CbfPacketBuffer::TimerCallback CbfPacketBufferTest::callback()
{
    return [this](CbfPacket::Data&& data) {
        ++calls;
        last_call_length = get_length(data);
    };
}


TEST_F(CbfPacketBufferTest, identifier_hash)
{
    std::hash<CbfPacketIdentifier> hasher;
    CbfPacketIdentifier id1 { Address {{ 1, 2, 3, 4, 5, 6}}, SequenceNumber(2) };
    CbfPacketIdentifier id2 { Address {{ 1, 2, 3, 4, 5, 6}}, SequenceNumber(3) };
    CbfPacketIdentifier id3 { Address {{ 1, 2, 3, 4, 5, 6}}, SequenceNumber(2) };
    CbfPacketIdentifier id4 { Address {{ 1, 2, 3, 4, 5, 7}}, SequenceNumber(3) };
    EXPECT_EQ(id1, id3);
    EXPECT_EQ(hasher(id1), hasher(id3));
    EXPECT_NE(id1, id2);
    EXPECT_NE(hasher(id1), hasher(id2));
    EXPECT_NE(id2, id4);
    EXPECT_NE(hasher(id2), hasher(id4));
}

TEST_F(CbfPacketBufferTest, packet_identifier)
{
    const MacAddress mac { 1, 3, 5, 7, 9, 11 };
    CbfPacket packet = create_packet(mac, 8);
    EXPECT_EQ(mac, packet.source().mid());
    EXPECT_EQ(SequenceNumber { 8 }, packet.sequence_number());
}

TEST_F(CbfPacketBufferTest, packet_counter)
{
    CbfPacket packet = create_packet({}, 1);

    // check correct initialization
    EXPECT_EQ(1, packet.counter());

    // counter should be modifiable
    ++packet.counter();
    EXPECT_EQ(2, packet.counter());
}

TEST_F(CbfPacketBufferTest, packet_lifetime)
{
    CbfPacket packet = create_packet({}, 1);

    // check initialization
    EXPECT_EQ(mib.itsGnDefaultPacketLifetime, packet.lifetime());

    // lifetime has to be modifiable
    packet.lifetime().set(Lifetime::Base::_1_S, 42);
    EXPECT_EQ((Lifetime {Lifetime::Base::_1_S, 42}), packet.lifetime());
}

TEST_F(CbfPacketBufferTest, packet_length)
{
    CbfPacket packet = create_packet({0, 1, 2, 3, 4, 5}, 3);
    EXPECT_EQ(GbcPduLength, packet.length());

    packet = create_packet({0, 1, 2, 3, 4, 5}, 3, 64);
    EXPECT_EQ(64, packet.length());
}

TEST_F(CbfPacketBufferTest, find)
{
    CbfPacketBuffer buffer(runtime, callback(), 8192);
    auto found1 = buffer.find(Address {{1, 2, 3, 4, 5, 6}}, SequenceNumber(3));
    EXPECT_FALSE(found1);

    auto packet1 = create_packet({1, 2, 3, 4, 5, 6}, 3);
    buffer.enqueue(std::move(packet1), std::chrono::seconds(5));

    auto found2 = buffer.find(Address {{1, 2, 3, 4, 5, 6}}, SequenceNumber(4));
    EXPECT_FALSE(found2);
    auto found3 = buffer.find(Address {{1, 2, 3, 4, 5, 6}}, SequenceNumber(3));
    ASSERT_TRUE(found3);
    EXPECT_EQ(1, found3->counter());
    EXPECT_EQ((MacAddress {1, 2, 3, 4, 5, 6}), found3->source().mid());
}

TEST_F(CbfPacketBufferTest, fetch)
{
    CbfPacketBuffer buffer(runtime, callback(), 8192);
    auto found1 = buffer.fetch(Address {{1, 2, 3, 4, 5, 6}}, SequenceNumber(3));
    EXPECT_FALSE(!!found1);

    auto packet1 = create_packet({1, 2, 3, 4, 5, 6}, 3);
    packet1.counter() = 3;
    buffer.enqueue(std::move(packet1), std::chrono::milliseconds(500));

    auto found2 = buffer.fetch(Address {{1, 2, 3, 4, 5, 6}}, SequenceNumber(4));
    EXPECT_FALSE(!!found2);
    auto found3 = buffer.fetch(Address {{1, 2, 3, 4, 5, 6}}, SequenceNumber(3));
    ASSERT_TRUE(!!found3);

    EXPECT_EQ(3, found3->counter());
    EXPECT_EQ((MacAddress {1, 2, 3, 4, 5, 6}), found3->source().mid());

    auto found4 = buffer.fetch(Address {{1, 2, 3, 4, 5, 6}}, SequenceNumber(3));
    EXPECT_FALSE(!!found4);
}

TEST_F(CbfPacketBufferTest, fetch_reduce_lifetime)
{
    CbfPacketBuffer buffer(runtime, callback(), 8192);
    auto packet = create_packet({1, 2, 3, 4, 5, 6}, 1);
    packet.lifetime().encode(1.0 * units::si::seconds);
    buffer.enqueue(std::move(packet), std::chrono::milliseconds(500));
    runtime.trigger(std::chrono::milliseconds(200));
    auto found = buffer.fetch(Address {{1, 2, 3, 4, 5, 6}}, SequenceNumber(1));
    ASSERT_TRUE(!!found);
    EXPECT_EQ((Lifetime {Lifetime::Base::_50_MS, 16}), found->lifetime());
}

TEST_F(CbfPacketBufferTest, next_timer_expiry)
{
    const Address addr {{1, 2, 3, 4, 5, 6}};

    CbfPacketBuffer buffer(runtime, callback(), 8192);
    EXPECT_EQ(Clock::time_point::max(), runtime.next());

    buffer.enqueue(create_packet(), std::chrono::seconds(3));
    EXPECT_EQ(std::chrono::seconds(3), runtime.next() - runtime.now());

    runtime.trigger(std::chrono::seconds(1));
    buffer.enqueue(create_packet(addr.mid(), 3), std::chrono::seconds(1));
    EXPECT_EQ(std::chrono::seconds(1), runtime.next() - runtime.now());

    buffer.enqueue(create_packet(addr.mid(), 2), std::chrono::milliseconds(200));
    EXPECT_EQ(std::chrono::milliseconds(200), runtime.next() - runtime.now());

    runtime.trigger(std::chrono::milliseconds(100));
    auto fetch = buffer.fetch(addr, SequenceNumber(2));
    EXPECT_TRUE(!!fetch);
    EXPECT_EQ(std::chrono::milliseconds(900), runtime.next() - runtime.now());

    bool dropped = buffer.try_drop(addr, SequenceNumber(3));
    EXPECT_TRUE(dropped);
    EXPECT_EQ(std::chrono::milliseconds(1900), runtime.next() - runtime.now());
}

TEST_F(CbfPacketBufferTest, try_drop_sequence_number)
{
    CbfPacketBuffer buffer(runtime, callback(), 8192);
    const auto addr = Address {{1, 1, 1, 1, 1, 1}};
    EXPECT_FALSE(buffer.try_drop(addr, SequenceNumber(3)));

    auto packet = create_packet(addr.mid(), 8);
    buffer.enqueue(std::move(packet), std::chrono::milliseconds(400));
    EXPECT_FALSE(buffer.try_drop(addr, SequenceNumber(7)));
    EXPECT_FALSE(buffer.try_drop(addr, SequenceNumber(9)));
    EXPECT_TRUE(buffer.try_drop(addr, SequenceNumber(8)));
    EXPECT_FALSE(buffer.try_drop(addr, SequenceNumber(8)));
}

TEST_F(CbfPacketBufferTest, try_drop_addr)
{
    CbfPacketBuffer buffer(runtime, callback(), 8192);
    const auto addr1 = Address {{1, 1, 1, 1, 1, 1}};
    const auto addr2 = Address {{ 2, 2, 2, 2, 2, 2}};

    auto packet = create_packet(addr1.mid(), 8);
    buffer.enqueue(std::move(packet), std::chrono::milliseconds(400));
    EXPECT_FALSE(buffer.try_drop(addr2, SequenceNumber(8)));
    EXPECT_TRUE(buffer.try_drop(addr1, SequenceNumber(8)));
    EXPECT_FALSE(buffer.try_drop(addr1, SequenceNumber(8)));
}

TEST_F(CbfPacketBufferTest, try_drop_multiple_packets)
{
    CbfPacketBuffer buffer(runtime, callback(), 8192);
    const auto addr = Address {{1, 1, 1, 1, 1, 1}};
    const auto timeout = std::chrono::milliseconds(400);

    auto packet1 = create_packet(addr.mid(), 8);
    buffer.enqueue(std::move(packet1), timeout);

    auto packet2 = create_packet(addr.mid(), 10);
    buffer.enqueue(std::move(packet2), timeout);

    EXPECT_FALSE(buffer.try_drop(addr, SequenceNumber(9)));
    EXPECT_TRUE(buffer.try_drop(addr, SequenceNumber(10)));
    EXPECT_TRUE(buffer.try_drop(addr, SequenceNumber(8)));
}

TEST_F(CbfPacketBufferTest, capacity)
{
    CbfPacketBuffer buffer(runtime, callback(), 256);

    buffer.enqueue(create_packet(128), std::chrono::seconds(1));
    buffer.enqueue(create_packet(128), std::chrono::seconds(1));
    runtime.trigger(std::chrono::milliseconds(1010));
    EXPECT_EQ(2, calls);

    buffer.enqueue(create_packet(157), std::chrono::seconds(1));
    buffer.enqueue(create_packet(100), std::chrono::seconds(1));
    runtime.trigger(std::chrono::seconds(2));
    EXPECT_EQ(3, calls);
    EXPECT_EQ(100, last_call_length);
}

TEST_F(CbfPacketBufferTest, packets_to_send)
{
    std::vector<CbfPacket::Data> packets;
    auto cb = [&packets](CbfPacket::Data&& data) { packets.emplace_back(std::move(data)); };
    CbfPacketBuffer buffer(runtime, cb, 8192);

    runtime.trigger(std::chrono::minutes(42));
    EXPECT_EQ(0, packets.size());

    buffer.enqueue(create_packet(110), std::chrono::seconds(5));
    runtime.trigger(std::chrono::seconds(1));
    EXPECT_EQ(0, packets.size());

    buffer.enqueue(create_packet(120), std::chrono::seconds(3));
    runtime.trigger(std::chrono::seconds(3));
    ASSERT_EQ(1, packets.size());
    EXPECT_EQ(120, get_length(packets[0]));

    runtime.trigger(std::chrono::seconds(3));
    EXPECT_EQ(2, packets.size());

    buffer.enqueue(create_packet(130), std::chrono::seconds(1));
    buffer.enqueue(create_packet(140), std::chrono::milliseconds(1500));
    runtime.trigger(std::chrono::seconds(2));
    ASSERT_EQ(4, packets.size());
    EXPECT_EQ(130, get_length(packets[2]));
    EXPECT_EQ(140, get_length(packets[3]));

    // check if lifetime is reduced by queuing time
    auto packet = create_packet(150);
    packet.lifetime().set(Lifetime::Base::_50_MS, 8); // 400ms lifetime
    buffer.enqueue(std::move(packet), std::chrono::milliseconds(72));
    runtime.trigger(runtime.next());
    ASSERT_EQ(5, packets.size());
    // Lifetime can only be encoded in 50ms steps (in best case)
    EXPECT_EQ((Lifetime {Lifetime::Base::_50_MS, 7}), packets[4].pdu->basic().lifetime);
}
