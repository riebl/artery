#include <gtest/gtest.h>
#include <vanetza/common/clock.hpp>
#include <vanetza/geonet/mib.hpp>
#include <vanetza/security/security_entity.hpp>
#include <vanetza/security/tests/check_payload.hpp>

using namespace vanetza;
using vanetza::security::check;

class SecurityEntity : public ::testing::Test
{
protected:
    security::EncapRequest create_encap_request()
    {
        geonet::ManagementInformationBase mib;
        ChunkPacket send_payload;
        send_payload[OsiLayer::Application] = ByteBuffer {89, 27, 1, 4, 18, 85};

        security::EncapRequest encap_request;
        encap_request.plaintext_payload = send_payload;

        return encap_request;
    }

    Clock::time_point time_now;
};

TEST_F(SecurityEntity, test_sign_method)
{
    //create SecurityEntity
    security::SecurityEntity sec_ent(time_now);

    //create signed packet
    security::EncapRequest encap_request = create_encap_request();
    security::EncapConfirm encap_confirm;
    encap_confirm = sec_ent.encapsulate_packet(encap_request);

    //SecuredMessage should not be empty
    EXPECT_GT(get_size(encap_confirm.sec_packet), 0);

    //check payload type
    EXPECT_EQ(encap_confirm.sec_packet.payload.type, security::PayloadType::Signed);

    //check if bytebuffers are the same
    check(encap_request.plaintext_payload, encap_confirm.sec_packet.payload.data);
}

TEST_F(SecurityEntity, test_verify_method)
{
    //create SecurityEntity
    security::SecurityEntity sec_ent(time_now);

    //create signed packet
    security::EncapRequest encap_request = create_encap_request();
    security::EncapConfirm encap_confirm;
    encap_confirm = sec_ent.encapsulate_packet(encap_request);

    //create decap_request
    security::DecapRequest decap_request(encap_confirm.sec_packet);

    //create decap_confirm
    security::DecapConfirm decap_confirm;
    decap_confirm = sec_ent.decapsulate_packet(decap_request);

    //check ReportType of decap_confirm
    EXPECT_EQ(decap_confirm.report, security::ReportType::Success);
}

TEST_F(SecurityEntity, test_verify_method_fail)
{
    //create SecurityEntity
    security::SecurityEntity sec_ent(time_now);

    //create signed packet
    security::EncapRequest encap_request = create_encap_request();
    security::EncapConfirm encap_confirm;
    encap_confirm = sec_ent.encapsulate_packet(encap_request);
    security::SecuredMessage& secured_message = encap_confirm.sec_packet;

    //create decap_request of signed packet
    security::DecapRequest decap_request(secured_message);

    //create new (wrong) payload
    ByteBuffer wrong_payload { 7 };

    //replace correct payload with new payload
    secured_message.payload.data = CohesivePacket(wrong_payload, OsiLayer::Application);

    //create decap_confirm
    security::DecapConfirm decap_confirm;
    decap_confirm = sec_ent.decapsulate_packet(decap_request);

    //check ReportType of decap_confirm
    EXPECT_EQ(decap_confirm.report, security::ReportType::False_Signature);
}
