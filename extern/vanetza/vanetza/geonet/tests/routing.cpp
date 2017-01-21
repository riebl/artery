#include <vanetza/geonet/data_confirm.hpp>
#include <vanetza/geonet/data_request.hpp>
#include <vanetza/geonet/packet.hpp>
#include <vanetza/geonet/tests/network_topology.hpp>
#include <vanetza/net/mac_address.hpp>
#include <gtest/gtest.h>
#include <list>
#include <tuple>

using namespace vanetza;
using namespace vanetza::geonet;

// user literal for convenient length definition
vanetza::units::Length operator"" _m(long double length)
{
    return vanetza::units::Length(length * vanetza::units::si::meters);
}

using RoutingParam = std::tuple<NetworkTopology::PacketDuplicationMode, bool>;

class Routing : public ::testing::TestWithParam<RoutingParam>
{
protected:
    virtual void SetUp() override
    {
        net.set_duplication_mode(std::get<0>(GetParam()));
        net.get_mib().itsGnSecurity = std::get<1>(GetParam());

        cars[0] = {0x00, 0x02, 0x03, 0x04, 0x05, 0x06};
        cars[1] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
        cars[2] = {0x02, 0x02, 0x03, 0x04, 0x05, 0x06};
        cars[3] = {0x03, 0x02, 0x03, 0x04, 0x05, 0x06};
        cars[4] = {0x04, 0x02, 0x03, 0x04, 0x05, 0x06};
        cars[5] = {0x05, 0x02, 0x03, 0x04, 0x05, 0x06};

        // add all routers
        for (auto& car : cars) {
            net.add_router(car.second);
        }

        // add reachability for all routers
        net.add_reachability(cars[0], {cars[1], cars[2], cars[3], cars[5]});
        net.add_reachability(cars[1], {cars[0], cars[2]});
        net.add_reachability(cars[2], {cars[0], cars[1], cars[3], cars[5]});
        net.add_reachability(cars[3], {cars[0], cars[2], cars[4]});
        net.add_reachability(cars[4], {cars[3]});
        net.add_reachability(cars[5], {cars[2], cars[0]});

        // positioning of cars
        net.set_position(cars[0], CartesianPosition(0.0_m, 0.0_m));
        net.set_position(cars[1], CartesianPosition(2.0_m, 0.0_m));
        net.set_position(cars[2], CartesianPosition(6.0_m, 0.0_m));
        net.set_position(cars[3], CartesianPosition(6.0_m, 6.0_m));
        net.set_position(cars[4], CartesianPosition(20.0_m, 6.0_m));
        net.set_position(cars[5], CartesianPosition(2.0_m, -1.0_m));

        // advance time 5 seconds
        net.advance_time(std::chrono::seconds(5));
        net.reset_counters();
    }

    std::unique_ptr<DownPacket> create_packet(ByteBuffer&& payload = {47, 11, 1, 4, 42, 85})
    {
        std::unique_ptr<DownPacket> packet { new DownPacket() };
        packet->layer(OsiLayer::Transport) = ByteBuffer(std::move(payload));
        return packet;
    }

    std::unordered_map<int, MacAddress> cars;
    NetworkTopology net;
};

/**
 * Check location table entries after initialisation
 * Expectation: Entries should reflect defined network reachability
 */
TEST_P(Routing, beacon_location_table) {
    auto& sender_table = net.get_router(cars[0])->get_location_table();
    EXPECT_FALSE(sender_table.has_entry(Address { cars[0] }));
    EXPECT_TRUE(sender_table.has_entry(Address { cars[1] }));
    EXPECT_TRUE(sender_table.has_entry(Address { cars[2] }));
    EXPECT_TRUE(sender_table.has_entry(Address { cars[3] }));
    EXPECT_FALSE(sender_table.has_entry(Address { cars[4] }));
    ASSERT_TRUE(sender_table.has_entry(Address { cars[5] }));
    const LocationTableEntry* entry5 = sender_table.get_entry(Address { cars[5] });
    ASSERT_TRUE(entry5);
    EXPECT_LT(0, entry5->position_vector.longitude.value());
    EXPECT_GT(0, entry5->position_vector.latitude.value());
}

/*
 * Preconditions:
 * - GeoAdhocRouter inside target area (INOUT1 >= 0)
 * - Packet not yet in CBF packet buffer (P not in B)
 * - LL address of GeoAdhoc Router is LL destination address (Dest_LL_ADDR == L_LL_ADDR)
 * Expectation: greedy forwarding
 */
TEST_P(Routing, advanced_forwarding__in_destarea__unbuffered__receiver_is_destination)
{
    GbcDataRequest gbc_request(net.get_mib());
    gbc_request.destination = circle_dest_area(1.0_m, 2.0_m, 0.0_m);
    gbc_request.upper_protocol = UpperProtocol::IPv6;
    auto confirm = net.get_router(cars[0])->request(gbc_request, create_packet());
    ASSERT_TRUE(confirm.accepted());
    EXPECT_EQ(cars[1], net.get_interface(cars[0])->last_request.destination);

    gbc_request.destination = circle_dest_area(1.0_m, 6.0_m, 0.0_m);
    confirm = net.get_router(cars[0])->request(gbc_request, create_packet());
    ASSERT_TRUE(confirm.accepted());
    EXPECT_EQ(cars[2], net.get_interface(cars[0])->last_request.destination);

    gbc_request.destination = circle_dest_area(1.0_m, 6.0_m, 6.0_m);
    confirm = net.get_router(cars[0])->request(gbc_request, create_packet());
    ASSERT_TRUE(confirm.accepted());
    EXPECT_EQ(cars[3], net.get_interface(cars[0])->last_request.destination);
}

/*
 * Preconditions:
 * - GeoAdhocRouter inside target area (INOUT1 >= 0)
 * - Packet not yet in CBF packet buffer (P not in B)
 * - LL address of GeoAdhoc Router is not LL destination address (Dest_LL_ADDR != L_LL_ADDR)
 * Expectation: contention based forwarding
 */
TEST_P(Routing, advanced_forwarding__in_destarea__unbuffered__receiver_is_not_destination)
{
    GbcDataRequest gbc_request(net.get_mib());
    gbc_request.destination = circle_dest_area(10.0_m, 0.0_m, 0.0_m);
    gbc_request.upper_protocol = UpperProtocol::IPv6;
    net.get_interface(cars[0])->last_packet.reset();
    EXPECT_FALSE(!!net.get_interface(cars[0])->last_packet);
    auto confirm = net.get_router(cars[0])->request(gbc_request, create_packet());
    ASSERT_TRUE(confirm.accepted());

    net.advance_time(std::chrono::milliseconds(100));
    EXPECT_TRUE(!!net.get_interface(cars[0])->last_packet);
    EXPECT_EQ(cBroadcastMacAddress, net.get_interface(cars[0])->last_request.destination);
}

/*
 * Preconditions:
 * - GeoAdhocRouter inside target area (INOUT1 >= 0)
 * - Packet in CBF packet buffer (P in B)
 * - Counter >= Max_Counter
 * Expectation: remove packet from buffer, stop timer, discard packet
 */
TEST_P(Routing, advanced_forwarding__in_destarea__buffered__max_counter_exceeded)
{
    const int maxCounter = 3;

    GbcDataRequest gbc_request(net.get_mib());
    gbc_request.destination = circle_dest_area(1.0_m, 2.0_m, 0.0_m);
    gbc_request.upper_protocol = UpperProtocol::IPv6;
    auto confirm = net.get_router(cars[0])->request(gbc_request, create_packet());
    ASSERT_TRUE(confirm.accepted());
    EXPECT_EQ(cars[1], net.get_interface(cars[0])->last_request.destination);
    net.dispatch();
    auto& car1_cbf = net.get_router(cars[1])->get_cbf_buffer();
    auto found = car1_cbf.find(Address { cars[0] }, SequenceNumber(0));
    ASSERT_TRUE(found);
    EXPECT_EQ(1, found->counter());

    for (int i = 0; i < maxCounter - 1; i++) {
        net.send(cars[0], cars[1]);
        auto found = car1_cbf.find(Address { cars[0] }, SequenceNumber(0));
        ASSERT_TRUE(found);
        EXPECT_EQ(i + 2, found->counter());
    }

    net.send(cars[0], cars[1]);
    found = net.get_router(cars[1])->get_cbf_buffer().find(Address { cars[0] }, SequenceNumber(0));
    EXPECT_FALSE(found);
}

/*
 * Preconditions:
 * - GeoAdhocRouter inside target area (INOUT1 >= 0)
 * - Packet in CBF packet buffer (P in B)
 * - Counter < Max_Counter
 * - GeoAdhocRouter is inside sectorial area (INOUT2 >= 0)
 * Expectation: remove packet from buffer, stop timer, discard packet
 */
TEST_P(Routing, advanced_forwarding__in_destarea__buffered__max_counter_below__in_sectorial)
{
    EXPECT_FALSE(net.get_router(cars[5])->outside_sectorial_contention_area(cars[0], cars[2]));

    GbcDataRequest gbc_request(net.get_mib());
    gbc_request.destination = circle_dest_area(1.0_m, 2.0_m, -1.0_m);
    gbc_request.upper_protocol = UpperProtocol::IPv6;
    auto confirm = net.get_router(cars[2])->request(gbc_request, create_packet());
    ASSERT_TRUE(confirm.accepted());
    EXPECT_EQ(cars[5], net.get_interface(cars[2])->last_request.destination);
    net.dispatch();
    auto found = net.get_router(cars[5])->get_cbf_buffer().find(Address { cars[2] }, SequenceNumber(0));
    ASSERT_TRUE(found);
    EXPECT_EQ(1, found->counter());

    net.get_interface(cars[0])->last_packet.reset(new ChunkPacket(*net.get_interface(cars[2])->last_packet));
    net.send(cars[0], cars[5]);
    found = net.get_router(cars[5])->get_cbf_buffer().find(Address { cars[2] }, SequenceNumber(0));
    EXPECT_FALSE(found);
}

/*
 * Preconditions:
 * - GeoAdhocRouter inside target area (INOUT1 >= 0)
 * - Packet in CBF packet buffer (P in B)
 * - Counter < Max_Counter
 * - GeoAdhocRouter is outside sectorial area (INOUT2 >= 0)
 * Expectation: packet is buffered (counter++, start timer with timeout TO_CBF_GBC)
 */
TEST_P(Routing, advanced_forwarding__in_destarea__buffered__max_counter_below__out_sectorial)
{
    net.set_position(cars[5], CartesianPosition(20.0_m, -1.0_m));
    net.advance_time(std::chrono::seconds(5));
    EXPECT_TRUE(net.get_router(cars[5])->outside_sectorial_contention_area(cars[0], cars[2]));

    GbcDataRequest gbc_request(net.get_mib());
    gbc_request.destination = circle_dest_area(1.0_m, 20.0_m, -1.0_m);
    gbc_request.upper_protocol = UpperProtocol::IPv6;
    auto confirm = net.get_router(cars[2])->request(gbc_request, create_packet());
    ASSERT_TRUE(confirm.accepted());
    EXPECT_EQ(cars[5], net.get_interface(cars[2])->last_request.destination);
    net.dispatch();
    auto found = net.get_router(cars[5])->get_cbf_buffer().find(Address { cars[2] }, SequenceNumber(0));
    ASSERT_TRUE(found);
    EXPECT_EQ(1, found->counter());

    net.get_interface(cars[0])->last_packet.reset(new ChunkPacket(*net.get_interface(cars[2])->last_packet));
    net.send(cars[0], cars[5]);
    found = net.get_router(cars[5])->get_cbf_buffer().find(Address { cars[2] }, SequenceNumber(0));
    ASSERT_TRUE(found);
    EXPECT_EQ(2, found->counter());
}

/*
 * Preconditions:
 * - GeoAdhocRouter outside target area (INOUT1 < 0)
 * - sender position is reliable (PV_SE exists, PAI_SE is true)
 * - sender outside target area (INOUT3 < 0)
 * Expectation: greedy forwarding
 */
TEST_P(Routing, advanced_forwarding__out_destarea__sender_out_destarea)
{
    GbcDataRequest gbc_request(net.get_mib());
    gbc_request.destination = circle_dest_area(1.0_m, 2.0_m, 2.0_m);
    gbc_request.upper_protocol = UpperProtocol::IPv6;
    auto confirm = net.get_router(cars[0])->request(gbc_request, create_packet());
    ASSERT_TRUE(confirm.accepted());
    EXPECT_EQ(cars[1], net.get_interface(cars[0])->last_request.destination);

    gbc_request.destination = circle_dest_area(1.0_m, 6.0_m, -2.0_m);
    confirm = net.get_router(cars[0])->request(gbc_request, create_packet());
    ASSERT_TRUE(confirm.accepted());
    EXPECT_EQ(cars[2], net.get_interface(cars[0])->last_request.destination);

    gbc_request.destination = circle_dest_area(1.0_m, 6.0_m, 8.0_m);
    confirm = net.get_router(cars[0])->request(gbc_request, create_packet());
    ASSERT_TRUE(confirm.accepted());
    EXPECT_EQ(cars[3], net.get_interface(cars[0])->last_request.destination);

    gbc_request.destination = circle_dest_area(1.0_m, -2.0_m, 0.0_m);
    confirm = net.get_router(cars[0])->request(gbc_request, create_packet());
    ASSERT_TRUE(confirm.accepted());
    EXPECT_EQ(cBroadcastMacAddress, net.get_interface(cars[0])->last_request.destination);

    gbc_request.destination = circle_dest_area(1.0_m, 2.0_m, 2.0_m);
    confirm = net.get_router(cars[0])->request(gbc_request, create_packet());
    ASSERT_TRUE(confirm.accepted());
    EXPECT_EQ(cars[1], net.get_interface(cars[0])->last_request.destination);

    gbc_request.destination = circle_dest_area(1.0_m, 20.0_m, 0.0_m);
    confirm = net.get_router(cars[0])->request(gbc_request, create_packet());
    ASSERT_TRUE(confirm.accepted());
    EXPECT_EQ(cars[2], net.get_interface(cars[0])->last_request.destination);
}

/*
 * Preconditions:
 * - GeoAdhocRouter outside target area (INOUT1 < 0)
 * - sender position is reliable (PV_SE exists, PAI_SE is true)
 * - sender inside target area (INOUT3 < 0)
 * Expectation: discard packet
 */
TEST_P(Routing, advanced_forwarding__out_destarea__sender_in_destarea)
{
    GbcDataRequest gbc_request(net.get_mib());
    gbc_request.destination = circle_dest_area(1.0_m, 0.0_m, 0.0_m);
    gbc_request.upper_protocol = UpperProtocol::IPv6;
    auto confirm = net.get_router(cars[0])->request(gbc_request, create_packet());
    ASSERT_TRUE(confirm.accepted());
    net.dispatch();

    EXPECT_TRUE(net.get_counter_indications() != 0);
    auto found = net.get_router(cars[1])->get_cbf_buffer().find(Address { cars[0] }, SequenceNumber(0));
    EXPECT_FALSE(found);
    found = net.get_router(cars[2])->get_cbf_buffer().find(Address { cars[0] }, SequenceNumber(0));
    EXPECT_FALSE(found);
    found = net.get_router(cars[3])->get_cbf_buffer().find(Address { cars[0] }, SequenceNumber(0));
    EXPECT_FALSE(found);
}

/*
 * Preconditions:
 * - GeoAdhocRouter outside target area (INOUT1 >= 0)
 * - sender position is not reliable (PV_SE exists, PAI_SE is false)
 * Expectation: next hop is broadcast
 */
TEST_P(Routing, advanced_forwarding__out_destarea__senderpos_unreliable)
{
    auto sender = net.get_router(cars[0]);
    LongPositionVector lpv = sender->get_local_position_vector();
    lpv.position_accuracy_indicator = false;
    sender->update(lpv);
    net.advance_time(std::chrono::seconds(1));
    net.reset_counters();

    GbcDataRequest gbc_request(net.get_mib());
    gbc_request.destination = circle_dest_area(1.0_m, 18.0_m, 6.0_m);
    gbc_request.upper_protocol = UpperProtocol::IPv6;
    net.get_interface(cars[0])->last_packet.reset();
    EXPECT_FALSE(!!net.get_interface(cars[0])->last_packet);
    auto confirm = sender->request(gbc_request, create_packet());
    ASSERT_TRUE(confirm.accepted());
    EXPECT_TRUE(!!net.get_interface(cars[0])->last_packet);
    EXPECT_EQ(cars[3], net.get_interface(cars[0])->last_request.destination);

    ASSERT_EQ(0, net.get_counter_indications());
    ASSERT_EQ(1, net.get_counter_requests(cars[0]));
    net.dispatch();
    ASSERT_EQ(1, net.get_counter_indications());

    EXPECT_EQ(1, net.get_counter_requests(cars[0]));
    EXPECT_EQ(1, net.get_counter_requests(cars[3]));
    EXPECT_EQ(cBroadcastMacAddress, net.get_interface(cars[3])->last_request.destination);
}

/*
 * Preconditions:
 * - GeoAdhocRouter outside target area (INOUT1 >= 0)
 * - sender position is reliable (PV_SE exists, PAI_SE is true)
 * Expectation: next hop is unicast
 */
TEST_P(Routing, advanced_forwarding__out_destarea__senderpos_reliable)
{
    auto sender = net.get_router(cars[0]);
    LongPositionVector lpv = sender->get_local_position_vector();
    lpv.position_accuracy_indicator = true;
    sender->update(lpv);
    net.advance_time(std::chrono::seconds(1));
    net.reset_counters();

    GbcDataRequest gbc_request(net.get_mib());
    gbc_request.destination = circle_dest_area(1.0_m, 18.0_m, 6.0_m);
    gbc_request.upper_protocol = UpperProtocol::IPv6;
    net.get_interface(cars[0])->last_packet.reset();
    EXPECT_FALSE(!!net.get_interface(cars[0])->last_packet);
    auto confirm = sender->request(gbc_request, create_packet());
    ASSERT_TRUE(confirm.accepted());
    EXPECT_TRUE(!!net.get_interface(cars[0])->last_packet);
    EXPECT_EQ(cars[3], net.get_interface(cars[0])->last_request.destination);

    ASSERT_EQ(0, net.get_counter_indications());
    ASSERT_EQ(1, net.get_counter_requests(cars[0]));
    net.dispatch();
    ASSERT_EQ(1, net.get_counter_indications());

    EXPECT_EQ(1, net.get_counter_requests(cars[0]));
    EXPECT_EQ(1, net.get_counter_requests(cars[3]));
    EXPECT_EQ(cars[4], net.get_interface(cars[3])->last_request.destination);
}

INSTANTIATE_TEST_CASE_P(RoutingInstantiation, Routing,
        ::testing::Combine(
            ::testing::Values(
                NetworkTopology::PacketDuplicationMode::COPY_CONSTRUCT,
                NetworkTopology::PacketDuplicationMode::SERIALIZE),
            ::testing::Bool()
        )
);
