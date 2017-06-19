#include <gtest/gtest.h>
#include <vanetza/btp/header.hpp>
#include <vanetza/common/runtime.hpp>
#include <vanetza/geonet/tests/fake_interfaces.hpp>
#include <vanetza/geonet/tests/security_context.hpp>
#include <vanetza/geonet/router.hpp>

using namespace vanetza;

class RouterIndicate : public ::testing::Test
{
public:
    RouterIndicate() :
        security(runtime), router(runtime, mib), packet_drop_occurred(false) {}

protected:
    virtual void SetUp() override
    {
        runtime.trigger(Clock::at("2010-12-23 18:30"));
        geonet::Address gn_addr;
        gn_addr.mid(MacAddress { 0, 0, 0, 0, 0, 1});
        router.set_address(gn_addr);
        router.set_access_interface(&req_ifc);
        router.set_transport_handler(geonet::UpperProtocol::IPv6, &ind_ifc);
        router.set_security_entity(&security.entity());
        packet_drop_occurred = false;
        router.packet_dropped = [this](geonet::Router::PacketDropReason r) {
            drop_reason = r;
            packet_drop_occurred = true;
        };
        test_payload_trans = {47, 11, 1, 4, 42, 85};
        test_payload_sess = {55, 1, 16, 45, 2, 65};
        test_payload_pres = {33, 2, 6, 27, 75, 1};
        send_payload.insert(send_payload.end(), test_payload_trans.begin(), test_payload_trans.end());
        send_payload.insert(send_payload.end(), test_payload_sess.begin(), test_payload_sess.end());
        send_payload.insert(send_payload.end(), test_payload_pres.begin(), test_payload_pres.end());
    }

    std::unique_ptr<geonet::DownPacket> create_packet()
    {
        std::unique_ptr<geonet::DownPacket> packet { new geonet::DownPacket() };
        packet->layer(OsiLayer::Transport) = ByteBuffer(test_payload_trans);
        packet->layer(OsiLayer::Session) = ByteBuffer(test_payload_sess);
        packet->layer(OsiLayer::Presentation) = ByteBuffer(test_payload_pres);
        return packet;
    }

    std::unique_ptr<geonet::UpPacket> get_up_packet(ByteBuffer& sec_packet_buffer)
    {
        // parse the data into UpPacket
        std::unique_ptr<geonet::UpPacket> up_packet(new geonet::UpPacket(CohesivePacket(sec_packet_buffer, OsiLayer::Network)));
        return up_packet;
    }

    ByteBuffer create_secured_packet()
    {
        // enable security
        mib.itsGnSecurity = true;

        // create ShbDataRequest
        geonet::ShbDataRequest request(mib, security::Profile::CAM);
        request.upper_protocol = geonet::UpperProtocol::IPv6;

        // Router handles request
        auto confirm = router.request(request, create_packet());
        assert(confirm.accepted());

        // secured packet on network layer
        ByteBuffer sec_packet_buffer;
        req_ifc.m_last_packet->layer(OsiLayer::Network).convert(sec_packet_buffer);
        assert(req_ifc.m_last_packet->size(OsiLayer::Transport, max_osi_layer()) == 0);

        assert(!sec_packet_buffer.empty());
        return sec_packet_buffer;
    }

    ByteBuffer create_plain_packet()
    {
        // disable security
        mib.itsGnSecurity = false;

        // create ShbDataRequest
        geonet::ShbDataRequest request(mib, security::Profile::CAM);
        request.upper_protocol = geonet::UpperProtocol::IPv6;

        // Router handles request
        auto confirm = router.request(request, create_packet());
        assert(confirm.accepted());

        // secured packet on network layer
        ByteBuffer plain_packet_buffer;
        for (auto layer : osi_layer_range<OsiLayer::Network, max_osi_layer()>()) {
            ByteBuffer layer_buffer;
            req_ifc.m_last_packet->layer(layer).convert(layer_buffer);
            plain_packet_buffer.insert(plain_packet_buffer.end(), layer_buffer.begin(), layer_buffer.end());
        }

        assert(!plain_packet_buffer.empty());
        return plain_packet_buffer;
    }

    bool test_and_reset_packet_drop()
    {
        bool result = packet_drop_occurred;
        packet_drop_occurred = false;
        return result;
    }

    MacAddress mac_address_sender = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    MacAddress mac_address_destination =  {0x07, 0x08, 0x09, 0x00, 0x01, 0x02};
    geonet::ManagementInformationBase mib;
    Runtime runtime;
    SecurityContext security;
    geonet::Router router;
    geonet::Router::PacketDropReason drop_reason;
    FakeRequestInterface req_ifc;
    FakeTransportInterface ind_ifc;
    ByteBuffer test_payload_trans;
    ByteBuffer test_payload_sess;
    ByteBuffer test_payload_pres;
    ByteBuffer send_payload;

private:
    bool packet_drop_occurred;
};

TEST_F(RouterIndicate, shb_unsecured_equal_payload)
{
    // create shb-up-packet by calling request
    ByteBuffer sec_packet_buffer = create_plain_packet();
    std::unique_ptr<geonet::UpPacket> packet_up = get_up_packet(sec_packet_buffer);

    // call indicate
    router.indicate(std::move(packet_up), mac_address_sender, mac_address_destination);

    // check hook, it shouldn't have been called
    EXPECT_FALSE(test_and_reset_packet_drop());

    // check if packet was not discarded
    ASSERT_NE(nullptr, ind_ifc.m_last_packet.get());
    // prepare a packet to check it's payload
    CohesivePacket* received_payload_ptr = boost::get<CohesivePacket>(ind_ifc.m_last_packet.get());
    ASSERT_NE(nullptr, received_payload_ptr);
    // extract received payload
    auto received_payload_range = (*received_payload_ptr)[OsiLayer::Transport];
    const ByteBuffer received_payload = ByteBuffer(received_payload_range.begin(), received_payload_range.end());
    // check payload
    EXPECT_EQ(send_payload, received_payload);
}

TEST_F(RouterIndicate, shb_secured_equal_payload)
{
    // create shb-up-packet by calling request
    ByteBuffer sec_packet_buffer = create_secured_packet();
    std::unique_ptr<geonet::UpPacket> packet_up = get_up_packet(sec_packet_buffer);

    // call indicate
    router.indicate(std::move(packet_up), mac_address_sender, mac_address_destination);

    // check hook, it shouldn't have been called
    EXPECT_FALSE(test_and_reset_packet_drop());

    // check if packet was not discarded
    ASSERT_NE(nullptr, ind_ifc.m_last_packet.get());
    // prepare a packet to check it's payload
    CohesivePacket* received_payload_ptr = boost::get<CohesivePacket>(ind_ifc.m_last_packet.get());
    ASSERT_NE(nullptr, received_payload_ptr);
    // extract received payload
    auto received_payload_range = (*received_payload_ptr)[OsiLayer::Transport];
    const ByteBuffer received_payload = ByteBuffer(received_payload_range.begin(), received_payload_range.end());
    // check payload
    EXPECT_EQ(send_payload, received_payload);
}

TEST_F(RouterIndicate, shb_secured_hook_its_protocol_version)
{
    // modify up_packet for negative test
    ByteBuffer broken_packet_buffer = create_secured_packet();
    broken_packet_buffer[0] ^= 0xff;

    std::unique_ptr<geonet::UpPacket> broken_packet_up = get_up_packet(broken_packet_buffer);
    router.indicate(std::move(broken_packet_up), mac_address_sender, mac_address_destination);

    // check hook
    EXPECT_TRUE(test_and_reset_packet_drop());
    EXPECT_EQ(geonet::Router::PacketDropReason::ITS_PROTOCOL_VERSION, drop_reason);

    // check if packet was dropped
    EXPECT_EQ(nullptr, ind_ifc.m_last_packet.get());
}

TEST_F(RouterIndicate, shb_secured_hook_parse_basic_header)
{
    // modify up_packet for negative test
    ByteBuffer broken_packet_buffer = create_secured_packet();
    broken_packet_buffer.erase(broken_packet_buffer.begin() + 3, broken_packet_buffer.end());

    std::unique_ptr<geonet::UpPacket> broken_packet_up = get_up_packet(broken_packet_buffer);
    router.indicate(std::move(broken_packet_up), mac_address_sender, mac_address_destination);

    // check hook
    EXPECT_TRUE(test_and_reset_packet_drop());
    EXPECT_EQ(geonet::Router::PacketDropReason::PARSE_BASIC_HEADER, drop_reason);

    // check if packet was dropped
    EXPECT_EQ(nullptr, ind_ifc.m_last_packet.get());
}

TEST_F(RouterIndicate, shb_secured_hook_parse_secured_header)
{
    // modify up_packet for negative test
    ByteBuffer broken_packet_buffer = create_secured_packet();
    broken_packet_buffer[geonet::BasicHeader::length_bytes] = 0x01;

    std::unique_ptr<geonet::UpPacket> broken_packet_up = get_up_packet(broken_packet_buffer);
    router.indicate(std::move(broken_packet_up), mac_address_sender, mac_address_destination);

    // check hook
    EXPECT_TRUE(test_and_reset_packet_drop());
    EXPECT_EQ(geonet::Router::PacketDropReason::PARSE_SECURED_HEADER, drop_reason);

    // check if packet was dropped
    EXPECT_EQ(nullptr, ind_ifc.m_last_packet.get());
}

TEST_F(RouterIndicate, shb_secured_hook_decap_unsuccessful_non_strict)
{
    mib.itsGnSnDecapResultHandling = geonet::SecurityDecapHandling::NON_STRICT;

    // modify up_packet for positive test
    ByteBuffer broken_packet_buffer = create_secured_packet();
    broken_packet_buffer[broken_packet_buffer.size() - 1] ^= 0xff;

    std::unique_ptr<geonet::UpPacket> broken_packet_up = get_up_packet(broken_packet_buffer);
    router.indicate(std::move(broken_packet_up), mac_address_sender, mac_address_destination);

    // check hook
    EXPECT_FALSE(test_and_reset_packet_drop());

    // check if packet arrived at transport layer
    EXPECT_EQ(1, ind_ifc.m_indications);
    EXPECT_NE(nullptr, ind_ifc.m_last_packet.get());
}

TEST_F(RouterIndicate, shb_secured_hook_decap_unsuccessful_strict)
{
    mib.itsGnSnDecapResultHandling = geonet::SecurityDecapHandling::STRICT;

    // modify up_packet for negative test
    ByteBuffer broken_packet_buffer = create_secured_packet();
    broken_packet_buffer[broken_packet_buffer.size() - 1] ^= 0xff;

    std::unique_ptr<geonet::UpPacket> broken_packet_up = get_up_packet(broken_packet_buffer);
    router.indicate(std::move(broken_packet_up), mac_address_sender, mac_address_destination);

    // check hook
    EXPECT_TRUE(test_and_reset_packet_drop());
    EXPECT_EQ(geonet::Router::PacketDropReason::DECAP_UNSUCCESSFUL_STRICT, drop_reason);

    // check if packet was dropped
    EXPECT_EQ(nullptr, ind_ifc.m_last_packet.get());
}

TEST_F(RouterIndicate, shb_secured_hook_parse_extended_header)
{
    // modify up_packet for negative test
    ByteBuffer broken_packet_buffer = create_plain_packet();
    // cut extended header partly off (18 bytes payload)
    ASSERT_LT(32, broken_packet_buffer.size());
    broken_packet_buffer.erase(broken_packet_buffer.end() - 32, broken_packet_buffer.end());

    std::unique_ptr<geonet::UpPacket> broken_packet_up = get_up_packet(broken_packet_buffer);
    router.indicate(std::move(broken_packet_up), mac_address_sender, mac_address_destination);

    // check hook
    EXPECT_TRUE(test_and_reset_packet_drop());
    EXPECT_EQ(geonet::Router::PacketDropReason::PARSE_EXTENDED_HEADER, drop_reason);

    // check if packet was dropped
    EXPECT_EQ(nullptr, ind_ifc.m_last_packet.get());
}

TEST_F(RouterIndicate, shb_secured_hook_payload_size)
{
    // modify up_packet for negative test
    ByteBuffer broken_packet_buffer = create_plain_packet();
    // cut payload partly off (18 bytes payload)
    ASSERT_LT(7, broken_packet_buffer.size());
    broken_packet_buffer.erase(broken_packet_buffer.end() - 7, broken_packet_buffer.end());

    std::unique_ptr<geonet::UpPacket> broken_packet_up = get_up_packet(broken_packet_buffer);
    router.indicate(std::move(broken_packet_up), mac_address_sender, mac_address_destination);

    // check hook
    EXPECT_TRUE(test_and_reset_packet_drop());
    EXPECT_EQ(geonet::Router::PacketDropReason::PAYLOAD_SIZE, drop_reason);

    // check if packet was dropped
    EXPECT_EQ(nullptr, ind_ifc.m_last_packet.get());
}

TEST_F(RouterIndicate, shb_secured_hook_hop_limit)
{
    // modify up_packet for negative test
    ByteBuffer broken_packet_buffer = create_secured_packet();
    // resest hop limit in basic header
    broken_packet_buffer[geonet::BasicHeader::length_bytes - 1] = mib.itsGnDefaultHopLimit + 1;

    std::unique_ptr<geonet::UpPacket> broken_packet_up = get_up_packet(broken_packet_buffer);
    router.indicate(std::move(broken_packet_up), mac_address_sender, mac_address_destination);

    // check hook
    EXPECT_TRUE(test_and_reset_packet_drop());
    EXPECT_EQ(geonet::Router::PacketDropReason::HOP_LIMIT, drop_reason);

    // check if packet was dropped
    EXPECT_EQ(nullptr, ind_ifc.m_last_packet.get());
}

