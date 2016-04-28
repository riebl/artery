#include <gtest/gtest.h>
#include <vanetza/btp/header_conversion.hpp>
#include <vanetza/btp/port_dispatcher.hpp>
#include <vanetza/btp/ports.hpp>
#include <vanetza/geonet/data_indication.hpp>
#include <vanetza/net/osi_layer.hpp>
#include <vanetza/net/packet.hpp>

using namespace vanetza;
using namespace vanetza::btp;
using vanetza::geonet::UpperProtocol;

class CounterIndicationInterface : public IndicationInterface
{
public:
    void indicate(const DataIndication&, std::unique_ptr<UpPacket>) override
    {
        ++counter;
    }

    unsigned counter = 0;
};

class CounterPromiscuousHook : public PortDispatcher::PromiscuousHook
{
public:
    void tap_packet(const DataIndication&, const UpPacket&) override
    {
        ++counter;
    }

    unsigned counter = 0;
};

class BtpPortDispatcherTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        undispatched_counter = 0;
        dispatcher.hook_undispatched =
            [this](const geonet::DataIndication&, const DataIndication*)
            {
                ++undispatched_counter;
            };
    }

    std::unique_ptr<UpPacket> create_btp_packet(port_type destination);
    geonet::DataIndication create_gn_indication(UpperProtocol protocol);

    using port_native = port_type::value_type;
    const port_native port_min = std::numeric_limits<port_native>::min();
    const port_native port_max = std::numeric_limits<port_native>::max();

    PortDispatcher dispatcher;
    std::size_t undispatched_counter;
};


TEST_F(BtpPortDispatcherTest, empty) {
    std::size_t indication_counter = 0;
    const auto port_range = port_max - port_min;

    // create list of port numbers to test
    std::list<port_native> ports;
    ports.push_back(port_min - 1);
    ports.push_back(port_min);
    ports.push_back(port_min + 1);
    ports.push_back(port_min + 0.2 * port_range);
    ports.push_back(port_min + 0.3 * port_range);
    ports.push_back(port_min + 0.5 * port_range);
    ports.push_back(port_min + 0.9 * port_range);
    ports.push_back(port_max - 1);
    ports.push_back(port_max);
    ports.push_back(port_max + 1);

    for (port_native p : ports) {
        dispatcher.indicate(
                create_gn_indication(UpperProtocol::BTP_B),
                create_btp_packet(host_cast(p))
        );
        EXPECT_EQ(undispatched_counter, ++indication_counter);
    }
}

TEST_F(BtpPortDispatcherTest, non_interactive) {
    ASSERT_EQ(undispatched_counter, 0);
    CounterIndicationInterface cam;
    CounterIndicationInterface denm;
    dispatcher.set_non_interactive_handler(ports::CAM, &cam);
    dispatcher.set_non_interactive_handler(ports::DENM, &denm);

    const geonet::DataIndication indication_this = create_gn_indication(UpperProtocol::BTP_B);
    const geonet::DataIndication indication_other = create_gn_indication(UpperProtocol::BTP_A);
    dispatcher.indicate(indication_other, create_btp_packet(ports::CAM));
    EXPECT_EQ(1, undispatched_counter);
    EXPECT_EQ(0, cam.counter);
    EXPECT_EQ(0, denm.counter);

    dispatcher.indicate(indication_this, create_btp_packet(ports::CAM));
    EXPECT_EQ(1, undispatched_counter);
    EXPECT_EQ(1, cam.counter);
    EXPECT_EQ(0, denm.counter);

    dispatcher.indicate(indication_this, create_btp_packet(ports::DENM));
    EXPECT_EQ(1, undispatched_counter);
    EXPECT_EQ(1, cam.counter);
    EXPECT_EQ(1, denm.counter);

    dispatcher.indicate(indication_this, create_btp_packet(ports::SPAT));
    EXPECT_EQ(2, undispatched_counter);
    EXPECT_EQ(1, cam.counter);
    EXPECT_EQ(1, denm.counter);
}

TEST_F(BtpPortDispatcherTest, non_interactive_overwrite) {
    ASSERT_EQ(undispatched_counter, 0);
    CounterIndicationInterface cam1_interface;
    CounterIndicationInterface cam2_interface;

    const auto indication = create_gn_indication(UpperProtocol::BTP_B);
    dispatcher.set_non_interactive_handler(ports::CAM, &cam1_interface);
    dispatcher.indicate(indication, create_btp_packet(ports::CAM));
    EXPECT_EQ(1, cam1_interface.counter);
    EXPECT_EQ(0, cam2_interface.counter);

    dispatcher.set_non_interactive_handler(ports::CAM, &cam2_interface);
    dispatcher.indicate(indication, create_btp_packet(ports::CAM));
    EXPECT_EQ(1, cam1_interface.counter);
    EXPECT_EQ(1, cam2_interface.counter);

    dispatcher.set_non_interactive_handler(ports::CAM, nullptr);
    dispatcher.indicate(indication, create_btp_packet(ports::CAM));
    EXPECT_EQ(1, cam2_interface.counter);
    EXPECT_EQ(1, undispatched_counter);
}

TEST_F(BtpPortDispatcherTest, promiscuous_hook) {
    CounterPromiscuousHook hook;
    const auto ind = create_gn_indication(UpperProtocol::BTP_B);

    dispatcher.indicate(ind, create_btp_packet(ports::CAM));
    EXPECT_EQ(0, hook.counter);

    dispatcher.add_promiscuous_hook(&hook);
    dispatcher.indicate(ind, create_btp_packet(ports::CAM));
    EXPECT_EQ(1, hook.counter);

    dispatcher.indicate(ind, create_btp_packet(ports::DENM));
    EXPECT_EQ(2, hook.counter);

    CounterIndicationInterface cam;
    dispatcher.set_non_interactive_handler(ports::CAM, &cam);
    dispatcher.indicate(ind, create_btp_packet(ports::CAM));
    EXPECT_EQ(1, cam.counter);
    EXPECT_EQ(3, hook.counter);

    dispatcher.add_promiscuous_hook(&hook);
    dispatcher.indicate(ind, create_btp_packet(ports::TOPO));
    EXPECT_EQ(5, hook.counter);
}


std::unique_ptr<UpPacket> BtpPortDispatcherTest::create_btp_packet(port_type destination)
{
    ChunkPacket packet;
    HeaderB header;
    header.destination_port = destination;
    packet[OsiLayer::Transport] = header;
    return std::unique_ptr<UpPacket> { new UpPacket(packet) };
}

geonet::DataIndication BtpPortDispatcherTest::create_gn_indication(UpperProtocol protocol)
{
    geonet::DataIndication indication;
    indication.upper_protocol = protocol;
    return indication;
}
