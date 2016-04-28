#include <gtest/gtest.h>
#include <vanetza/geonet/basic_header.hpp>
#include <vanetza/geonet/extended_pdu.hpp>
#include <vanetza/geonet/packet_buffer.hpp>
#include <vanetza/geonet/shb_header.hpp>
#include <chrono>

using namespace vanetza;
using namespace vanetza::geonet;

class FakeData : public packet_buffer::Data
{
public:
    FakeData() : m_state(NextHop::State::DISCARDED)
    {
        ++s_counter;
    }
    std::size_t length() const override { return m_length; }
    Lifetime& lifetime() override { return m_lifetime; }
    NextHop flush() override
    {
        NextHop nh;
        nh.mac(m_address);
        nh.state(m_state);

        if (m_state == NextHop::State::VALID) {
            std::unique_ptr<Pdu> pdu { new ExtendedPdu<ShbHeader>() };
            std::unique_ptr<DownPacket> payload { new DownPacket() };
            pdu->basic().lifetime = m_lifetime;
            payload->layer(OsiLayer::Application) = ByteBuffer(m_length);
            nh.data(std::move(pdu), std::move(payload));
        }

        return nh;
    }

    static std::unique_ptr<FakeData> create_valid_length(std::size_t length)
    {
        std::unique_ptr<FakeData> data { new FakeData() };
        data->m_length = length;
        data->m_state = NextHop::State::VALID;
        data->m_address = create_mac_address(s_counter);
        return data;
    }

    static std::unique_ptr<FakeData> create_valid_lifetime(units::Duration duration)
    {
        std::unique_ptr<FakeData> data { new FakeData() };
        data->m_length = 100;
        data->m_state = NextHop::State::VALID;
        data->m_address = create_mac_address(s_counter);
        data->lifetime().encode(duration);
        return data;
    }

    static std::unique_ptr<FakeData> create_with_state(NextHop::State s)
    {
        std::unique_ptr<FakeData> data { new FakeData() };
        data->m_length = 100;
        data->m_state = s;
        data->m_address = create_mac_address(s_counter);
        return data;
    }

    std::size_t m_length;
    Lifetime m_lifetime;
    MacAddress m_address;
    NextHop::State m_state;
    static unsigned s_counter;
};

unsigned FakeData::s_counter = 0;

TEST(PacketBuffer, push)
{
    Timestamp now;
    PacketBuffer buffer(8192);

    EXPECT_TRUE(buffer.push(FakeData::create_valid_length(5000), now));
    EXPECT_TRUE(buffer.push(FakeData::create_valid_length(5000), now));
    EXPECT_FALSE(buffer.push(FakeData::create_valid_length(8200), now));
}

TEST(PacketBuffer, flush_head_drop)
{
    Timestamp now;
    PacketBuffer buffer(8192);

    FakeData::s_counter = 0;
    buffer.push(FakeData::create_valid_length(2000), now);
    buffer.push(FakeData::create_valid_length(3000), now);
    buffer.push(FakeData::create_valid_length(4000), now);

    auto data = buffer.flush(now);
    ASSERT_EQ(2, data.size());
    EXPECT_EQ(create_mac_address(2), data.front().mac());
    EXPECT_EQ(create_mac_address(3), data.back().mac());

    // buffer shall be empty now
    EXPECT_EQ(0, buffer.flush(now).size());
}

TEST(PacketBuffer, flush_expired)
{
    Timestamp now;
    PacketBuffer buffer(8192);

    FakeData::s_counter = 0;
    buffer.push(FakeData::create_valid_lifetime(3.2 * units::si::seconds), now);
    now += 3000 * Timestamp::millisecond();
    buffer.push(FakeData::create_valid_lifetime(10.3 * units::si::seconds), now);
    buffer.push(FakeData::create_valid_lifetime(1.0 * units::si::seconds), now);

    now += 2000 * Timestamp::millisecond();
    auto data = buffer.flush(now);
    ASSERT_EQ(1, data.size());
    EXPECT_EQ(create_mac_address(2), data.front().mac());
}

TEST(PacketBuffer, flush_not_valid)
{
    Timestamp now;
    PacketBuffer buffer(8192);

    FakeData::s_counter = 0;
    buffer.push(FakeData::create_with_state(NextHop::State::VALID), now);
    buffer.push(FakeData::create_with_state(NextHop::State::DISCARDED), now);
    buffer.push(FakeData::create_with_state(NextHop::State::BUFFERED), now);
    buffer.push(FakeData::create_with_state(NextHop::State::VALID), now);

    auto data = buffer.flush(now);
    ASSERT_EQ(2, data.size());
    EXPECT_EQ(create_mac_address(1), data.front().mac());
    EXPECT_EQ(create_mac_address(4), data.back().mac());
}

TEST(PacketBuffer, update_lifetime)
{
    Timestamp now;
    PacketBuffer buffer(8192);

    buffer.push(FakeData::create_valid_lifetime(2.3 * units::si::seconds), now);
    now += 150 * Timestamp::millisecond();
    auto list = buffer.flush(now);
    ASSERT_EQ(1, list.size());
    auto data = list.front().data();
    EXPECT_EQ(2.15 * units::si::seconds, std::get<0>(data)->basic().lifetime.decode());
}
