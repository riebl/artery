#include <gtest/gtest.h>
#include <vanetza/common/runtime.hpp>
#include <vanetza/geonet/tests/fake_interfaces.hpp>
#include <vanetza/geonet/tests/security_context.hpp>
#include <vanetza/geonet/pdu_conversion.hpp>
#include <vanetza/geonet/pdu_variant.hpp>
#include <vanetza/geonet/router.hpp>
#include <vanetza/geonet/serialization_buffer.hpp>

using namespace vanetza;
using namespace vanetza::geonet;

class RouterRequest : public ::testing::Test
{
public:
    RouterRequest() :
        security(runtime), router(runtime, mib) {}

protected:
    virtual void SetUp() override
    {
        mib.itsGnSecurity = true;
        router.set_access_interface(&req_ifc);
        router.set_security_entity(&security.entity());
        router.set_transport_handler(geonet::UpperProtocol::IPv6, &ind_ifc);
        test_payload[OsiLayer::Application] = ByteBuffer {47, 11, 1, 4, 42, 85};
    }

    std::unique_ptr<geonet::DownPacket> create_packet()
    {
        std::unique_ptr<DownPacket> packet { new DownPacket(test_payload) };
        return packet;
    }

    ManagementInformationBase mib;
    Runtime runtime;
    SecurityContext security;
    Router router;
    FakeRequestInterface req_ifc;
    FakeTransportInterface ind_ifc;
    ChunkPacket test_payload;
};

TEST_F(RouterRequest, router_request)
{
    // create ShbDataRequest
    ShbDataRequest request(mib, security::Profile::CAM);
    request.upper_protocol = UpperProtocol::IPv6;

    // Router handles request
    auto confirm = router.request(request, create_packet());
    EXPECT_TRUE(confirm.accepted());

    // get the data from the fake network
    ByteBuffer net_payload;
    for (const auto layer : osi_layer_range<OsiLayer::Network, OsiLayer::Application>()) {
        ByteBuffer tmp;
        req_ifc.m_last_packet->layer(layer).convert(tmp);
        std::copy(tmp.begin(), tmp.end(), std::back_inserter(net_payload));
    }

    UpPacket packet_up { CohesivePacket(net_payload, OsiLayer::Network) };

    DownPacket packet_mac = *req_ifc.m_last_packet;
    // all data should be in network layer: payload is encapsulated by secured message
    EXPECT_EQ(packet_mac.size(), packet_mac[OsiLayer::Network].size());

    // prepare access to network layer's PDU
    using pdu_convertible = convertible::byte_buffer_impl<std::unique_ptr<Pdu>>;
    pdu_convertible* pdu_conv = dynamic_cast<pdu_convertible*>(packet_mac[OsiLayer::Network].ptr());
    ASSERT_TRUE(pdu_conv);
    auto pdu = pdu_conv->m_pdu.get();
    ASSERT_TRUE(pdu);
    auto pdu_ext = dynamic_cast<ShbPdu*>(pdu);
    ASSERT_TRUE(pdu_ext);

    // check if packet has secured part
    EXPECT_EQ(NextHeaderBasic::SECURED, pdu->basic().next_header);
    EXPECT_TRUE(pdu_ext->secured());
    auto secured = *pdu_ext->secured();

    // check payload of packet
    EXPECT_EQ(security::PayloadType::Signed, secured.payload.type);
    EXPECT_EQ(test_payload.size(), pdu->common().payload);
    const size_t payload_header_length = CommonHeader::length_bytes + ShbHeader::length_bytes;
    EXPECT_EQ(payload_header_length, size(secured.payload.data, OsiLayer::Network));
    EXPECT_EQ(test_payload.size(), size(secured.payload.data, OsiLayer::Transport, OsiLayer::Application));

    ChunkPacket sec_payload = boost::get<ChunkPacket>(secured.payload.data);
    ChunkPacket sec_header = sec_payload.extract(OsiLayer::Network, OsiLayer::Network);
    ByteBuffer actual_payload, expected_payload;
    serialize_into_buffer(sec_payload, actual_payload);
    serialize_into_buffer(test_payload, expected_payload);
    EXPECT_EQ(expected_payload, actual_payload);

    ByteBuffer actual_payload_header, expected_payload_header;
    geonet::serialize_into_buffer(pdu_ext->common(), expected_payload_header);
    geonet::serialize_into_buffer(pdu_ext->extended(), expected_payload_header);
    sec_header[OsiLayer::Network].convert(actual_payload_header);
    EXPECT_EQ(expected_payload_header, actual_payload_header);
}

TEST_F(RouterRequest, modified_request_maximum_lifetime)
{
    // create ShbDataRequest
    ShbDataRequest request(mib, security::Profile::CAM);

    // create new Lifetime that is larger than the itsGnMaxPacketLifetime of mib
    Lifetime large_lifetime(Lifetime::Base::_100_S, 9);

    request.maximum_lifetime = large_lifetime;
    request.upper_protocol = UpperProtocol::IPv6;

    // Router handles request
    auto confirm = router.request(request, create_packet());
    EXPECT_EQ(DataConfirm::ResultCode::REJECTED_MAX_LIFETIME, confirm.result_code);
}

TEST_F(RouterRequest, modified_request_repetition)
{
    // create ShbDataRequest
    ShbDataRequest request(mib, security::Profile::CAM);

    // create durations that will fail in data_confirm
    auto rep_faulty_int = 0.0 * units::si::seconds; // this has to be lower than mib.itsGnMinPacketRepetitionInterval
    auto rep_max = 99.0 * units::si::seconds;

    // create Repetition with faulty interval
    DataRequest::Repetition rep;
    rep.interval = rep_faulty_int;
    rep.maximum = rep_max;

    request.repetition = rep;
    request.upper_protocol = UpperProtocol::IPv6;

    // Router handles request
    auto confirm = router.request(request, create_packet());
    EXPECT_EQ(DataConfirm::ResultCode::REJECTED_MIN_REPETITION_INTERVAL, confirm.result_code);
}

TEST_F(RouterRequest, modified_request_payload_null)
{
    // create ShbDataRequest
    ShbDataRequest request(mib, security::Profile::CAM);
    request.upper_protocol = UpperProtocol::IPv6;

    // Router handles request
    auto confirm = router.request(request, nullptr);
    EXPECT_EQ(DataConfirm::ResultCode::REJECTED_UNSPECIFIED, confirm.result_code);
}

TEST_F(RouterRequest, modified_request_large_payload)
{
    std::unique_ptr<geonet::DownPacket> packet { new geonet::DownPacket() };

    // create too large payload
    ByteBuffer payload_large;
    for (int i = 0; i < 1000; i++) {
        ByteBuffer tmp = {0,1,2,3,4,5,6,7,8,9};
        payload_large.insert(payload_large.end(), tmp.begin(), tmp.end());
    }

    // insert payload in packet
    packet->layer(OsiLayer::Transport) = ByteBuffer(payload_large);

    // create ShbDataRequest
    ShbDataRequest request(mib, security::Profile::CAM);
    request.upper_protocol = UpperProtocol::IPv6;

    // Router handles request
    auto confirm = router.request(request, std::move(packet));
    EXPECT_EQ(DataConfirm::ResultCode::REJECTED_MAX_SDU_SIZE, confirm.result_code);
}

TEST_F(RouterRequest, shb_repetition)
{
    ShbDataRequest request(mib, security::Profile::CAM);
    request.repetition = DataRequest::Repetition {
        0.1 * units::si::seconds, 10.0 * units::si::seconds
    };
    EXPECT_EQ(0, req_ifc.m_requests);

    auto confirm = router.request(request, create_packet());
    EXPECT_EQ(DataConfirm::ResultCode::ACCEPTED, confirm.result_code);
    EXPECT_EQ(1, req_ifc.m_requests);
    ASSERT_TRUE(!!req_ifc.m_last_packet);
    const auto packet_size = size(*req_ifc.m_last_packet, OsiLayer::Network, OsiLayer::Application);

    // trigger five repetitions
    for (unsigned i = 0; i < 5; ++i) {
        runtime.trigger(std::chrono::milliseconds(100));
    }
    EXPECT_EQ(6, req_ifc.m_requests);
    EXPECT_EQ(packet_size, size(*req_ifc.m_last_packet, OsiLayer::Network, OsiLayer::Application));
}
