#include <gtest/gtest.h>
#include <vanetza/common/clock.hpp>
#include <vanetza/security/security_entity.hpp>
#include <vanetza/security/tests/check_payload.hpp>
#include <vanetza/security/tests/check_signature.hpp>

using namespace vanetza;
using namespace vanetza::security;

class SecurityEntityTest : public ::testing::Test
{
protected:
    SecurityEntityTest() : sec_ent(time_now)
    {
    }

    void SetUp() override
    {
        time_now = Clock::at("2016-03-7 08:23");
        expected_payload[OsiLayer::Transport] = ByteBuffer {89, 27, 1, 4, 18, 85};
    }

    EncapRequest create_encap_request()
    {
        EncapRequest encap_request;
        encap_request.plaintext_payload = expected_payload;
        encap_request.security_profile = Profile::CAM;
        return encap_request;
    }

    SecuredMessage create_secured_message()
    {
        EncapConfirm confirm = sec_ent.encapsulate_packet(create_encap_request());
        return confirm.sec_packet;
    }

    Clock::time_point time_now;
    SecurityEntity sec_ent;
    ChunkPacket expected_payload;
};

TEST_F(SecurityEntityTest, missing_backend)
{
    EXPECT_THROW(SecurityEntity invalid_backend(time_now, "Leberkas"), std::runtime_error);
}

TEST_F(SecurityEntityTest, mutual_acceptance)
{
    SecurityEntity other_sec_ent(time_now);
    EncapConfirm encap_confirm = other_sec_ent.encapsulate_packet(create_encap_request());
    DecapConfirm decap_confirm = sec_ent.decapsulate_packet(DecapRequest { encap_confirm.sec_packet });
    EXPECT_EQ(ReportType::Success, decap_confirm.report);
}

#if defined(VANETZA_WITH_CRYPTOPP) && defined(VANETZA_WITH_OPENSSL)
TEST_F(SecurityEntityTest, mutual_acceptance_impl)
{
    SecurityEntity cryptopp_sec_ent(time_now, "CryptoPP");
    SecurityEntity openssl_sec_ent(time_now, "OpenSSL");

    // OpenSSL to Crypto++
    EncapConfirm encap_confirm = openssl_sec_ent.encapsulate_packet(create_encap_request());
    DecapConfirm decap_confirm = cryptopp_sec_ent.decapsulate_packet(DecapRequest { encap_confirm.sec_packet });
    EXPECT_EQ(ReportType::Success, decap_confirm.report);

    // Crypto++ to OpenSSL
    encap_confirm = cryptopp_sec_ent.encapsulate_packet(create_encap_request());
    decap_confirm = openssl_sec_ent.decapsulate_packet(DecapRequest { encap_confirm.sec_packet });
    EXPECT_EQ(ReportType::Success, decap_confirm.report);
}
#endif

TEST_F(SecurityEntityTest, signed_payload_equals_plaintext_payload)
{
    EncapConfirm confirm = sec_ent.encapsulate_packet(create_encap_request());

    // check if sec_payload equals plaintext_payload
    check(expected_payload, confirm.sec_packet.payload.data);
}

TEST_F(SecurityEntityTest, signature_is_ecdsa)
{
    EncapConfirm confirm = sec_ent.encapsulate_packet(create_encap_request());

    // check if trailer_fields contain signature
    EXPECT_EQ(1, confirm.sec_packet.trailer_fields.size());
    auto signature = confirm.sec_packet.trailer_field(TrailerFieldType::Signature);
    ASSERT_TRUE(!!signature);
    auto signature_type = get_type(boost::get<Signature>(*signature));
    EXPECT_EQ(PublicKeyAlgorithm::Ecdsa_Nistp256_With_Sha256, signature_type);
}

TEST_F(SecurityEntityTest, expected_header_field_size)
{
    EncapConfirm confirm = sec_ent.encapsulate_packet(create_encap_request());

    // check header_field size
    EXPECT_EQ(3, confirm.sec_packet.header_fields.size());
}

TEST_F(SecurityEntityTest, expected_payload)
{
    EncapConfirm confirm = sec_ent.encapsulate_packet(create_encap_request());

    // check payload
    Payload payload = confirm.sec_packet.payload;
    EXPECT_EQ(expected_payload.size(), size(payload.data, min_osi_layer(), max_osi_layer()));
    EXPECT_EQ(PayloadType::Signed, get_type(payload));
}

TEST_F(SecurityEntityTest, verify_message)
{
    // prepare decap request
    auto secured_message = create_secured_message();
    DecapRequest decap_request(secured_message);

    // verify message
    DecapConfirm decap_confirm = sec_ent.decapsulate_packet(decap_request);

    // check if verify was successful
    EXPECT_EQ(ReportType::Success, decap_confirm.report);
    // check if payload was not changed
    check(expected_payload, decap_confirm.plaintext_payload);
    // check certificate validity
    EXPECT_TRUE(decap_confirm.certificate_validity);
}

TEST_F(SecurityEntityTest, verify_message_modified_message_type)
{
    // prepare decap request
    auto secured_message = create_secured_message();
    DecapRequest decap_request(secured_message);

    // iterate through all header_fields
    auto& header_fields = secured_message.header_fields;
    for (auto& field : header_fields) {
        // modify message type
        if (HeaderFieldType::Its_Aid == get_type(field)) {
            boost::get<IntX>(field).set(42);
        }
    }

    // verify message
    DecapConfirm decap_confirm = sec_ent.decapsulate_packet(decap_request);
    // check if verify was successful
    EXPECT_EQ(ReportType::False_Signature, decap_confirm.report);
}

TEST_F(SecurityEntityTest, verify_message_modified_certificate_name)
{
    // create decap request
    auto secured_message = create_secured_message();
    DecapRequest decap_request(secured_message);

    // iterate through all header_fields
    auto& header_fields = secured_message.header_fields;
    for (auto& field : header_fields) {
        // modify certificate
        if (HeaderFieldType::Signer_Info == get_type(field)) {
            SignerInfo& signer_info = boost::get<SignerInfo>(field);
            Certificate& certificate = boost::get<Certificate>(signer_info);

            // change the subject name
            certificate.subject_info.subject_name = {42};
        }
    }

    // verify message
    DecapConfirm decap_confirm = sec_ent.decapsulate_packet(decap_request);
    ASSERT_FALSE(decap_confirm.certificate_validity);
    EXPECT_EQ(CertificateInvalidReason::INVALID_NAME, decap_confirm.certificate_validity.reason());
}

TEST_F(SecurityEntityTest, verify_message_modified_certificate_signer_info)
{
    // create decap request
    auto secured_message = create_secured_message();
    DecapRequest decap_request(secured_message);

    // iterate through all header_fields
    auto& header_fields = secured_message.header_fields;
    for (auto& field : header_fields) {
        // modify certificate
        if (HeaderFieldType::Signer_Info == get_type(field)) {
            SignerInfo& signer_info = boost::get<SignerInfo>(field);
            Certificate& certificate = boost::get<Certificate>(signer_info);

            // change the subject info
            HashedId8 faulty_hash {{ 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 }};
            certificate.signer_info = faulty_hash;
        }
    }

    // verify message
    DecapConfirm decap_confirm = sec_ent.decapsulate_packet(decap_request);
    ASSERT_FALSE(decap_confirm.certificate_validity);
    EXPECT_EQ(CertificateInvalidReason::INVALID_ROOT_HASH, decap_confirm.certificate_validity.reason());
}

TEST_F(SecurityEntityTest, verify_message_modified_certificate_subject_info)
{
    // create decap request
    auto secured_message = create_secured_message();
    DecapRequest decap_request(secured_message);

    // iterate through all header_fields
    auto& header_fields = secured_message.header_fields;
    for (auto& field : header_fields) {
        // modify certificate
        if (HeaderFieldType::Signer_Info == get_type(field)) {
            SignerInfo& signer_info = boost::get<SignerInfo>(field);
            Certificate& certificate = boost::get<Certificate>(signer_info);

            // change the subject info
            certificate.subject_info.subject_type = SubjectType::Root_Ca;
        }
    }

    // verify message
    DecapConfirm decap_confirm = sec_ent.decapsulate_packet(decap_request);
    ASSERT_FALSE(decap_confirm.certificate_validity);
    EXPECT_EQ(CertificateInvalidReason::INVALID_SIGNATURE, decap_confirm.certificate_validity.reason());
}

TEST_F(SecurityEntityTest, verify_message_modified_certificate_subject_assurance)
{
    // create decap request
    auto secured_message = create_secured_message();
    DecapRequest decap_request(secured_message);

    // iterate through all header_fields
    auto& header_fields = secured_message.header_fields;
    for (auto& field : header_fields) {
        // modify certificate
        if (HeaderFieldType::Signer_Info == get_type(field)) {
            SignerInfo& signer_info = boost::get<SignerInfo>(field);
            Certificate& certificate = boost::get<Certificate>(signer_info);

            std::list<SubjectAttribute>& subject_attributes_list = certificate.subject_attributes;

            // iterate over subject_attribute list
            for (auto& subject_attribute : subject_attributes_list) {

                if (SubjectAttributeType::Assurance_Level == get_type(subject_attribute)) {
                    SubjectAssurance& subject_assurance = boost::get<SubjectAssurance>(subject_attribute);

                    // change raw in subject assurance to random value
                    subject_assurance.raw = 0x47;
                }
            }
        }
    }

    // verify message
    DecapConfirm decap_confirm = sec_ent.decapsulate_packet(decap_request);
    ASSERT_FALSE(decap_confirm.certificate_validity);
    EXPECT_EQ(CertificateInvalidReason::INVALID_SIGNATURE, decap_confirm.certificate_validity.reason());
}

TEST_F(SecurityEntityTest, verify_message_outdated_certificate)
{
    // prepare decap request
    auto secured_message = create_secured_message();

    // forge certificate with outdatet validity
    StartAndEndValidity outdated_validity;
    outdated_validity.start_validity = convert_time32(time_now - std::chrono::hours(1));
    outdated_validity.end_validity = convert_time32(time_now - std::chrono::minutes(1));
    auto signer_info_field = secured_message.header_field(HeaderFieldType::Signer_Info);
    ASSERT_TRUE(signer_info_field);
    auto certificate = boost::get<Certificate>(&boost::get<SignerInfo>(*signer_info_field));
    ASSERT_TRUE(certificate);
    certificate->validity_restriction.clear();
    certificate->validity_restriction.push_back(outdated_validity);

    // verify message
    DecapConfirm decap_confirm = sec_ent.decapsulate_packet(DecapRequest { secured_message });
    EXPECT_EQ(ReportType::Invalid_Certificate, decap_confirm.report);
    ASSERT_FALSE(decap_confirm.certificate_validity);
    EXPECT_EQ(CertificateInvalidReason::OFF_TIME_PERIOD, decap_confirm.certificate_validity.reason());
}

TEST_F(SecurityEntityTest, verify_message_premature_certificate)
{
    // prepare decap request
    auto secured_message = create_secured_message();

    // forge certificate with premature validity
    StartAndEndValidity premature_validity;
    premature_validity.start_validity = convert_time32(time_now + std::chrono::hours(1));
    premature_validity.end_validity = convert_time32(time_now + std::chrono::hours(25));
    auto signer_info_field = secured_message.header_field(HeaderFieldType::Signer_Info);
    ASSERT_TRUE(signer_info_field);
    auto certificate = boost::get<Certificate>(&boost::get<SignerInfo>(*signer_info_field));
    ASSERT_TRUE(certificate);
    certificate->validity_restriction.clear();
    certificate->validity_restriction.push_back(premature_validity);

    // verify message
    DecapConfirm decap_confirm = sec_ent.decapsulate_packet(DecapRequest { secured_message });
    EXPECT_EQ(ReportType::Invalid_Certificate, decap_confirm.report);
    ASSERT_FALSE(decap_confirm.certificate_validity);
    EXPECT_EQ(CertificateInvalidReason::OFF_TIME_PERIOD, decap_confirm.certificate_validity.reason());
}

TEST_F(SecurityEntityTest, verify_message_modified_certificate_validity_restriction)
{
    // prepare decap request
    auto secured_message = create_secured_message();
    DecapRequest decap_request(secured_message);

    // iterate through all header_fields
    auto& header_fields = secured_message.header_fields;
    for (auto& field : header_fields) {
        // modify certificate
        if (HeaderFieldType::Signer_Info == get_type(field)) {
            SignerInfo& signer_info = boost::get<SignerInfo>(field);
            Certificate& certificate = boost::get<Certificate>(signer_info);

            std::list<ValidityRestriction>& restriction_list = certificate.validity_restriction;

            // iterate over validity_restriction list
            for (auto& validity_restriction : restriction_list) {

                ValidityRestrictionType type = get_type(validity_restriction);
                ASSERT_EQ(type, ValidityRestrictionType::Time_Start_And_End);

                // change start and end time of certificate validity
                StartAndEndValidity& start_and_end = boost::get<StartAndEndValidity>(validity_restriction);
                start_and_end.start_validity = 500;
                start_and_end.end_validity = 373;
            }
        }
    }

    // verify message
    DecapConfirm decap_confirm = sec_ent.decapsulate_packet(decap_request);
    ASSERT_FALSE(decap_confirm.certificate_validity);
    EXPECT_EQ(CertificateInvalidReason::BROKEN_TIME_PERIOD, decap_confirm.certificate_validity.reason());
}

TEST_F(SecurityEntityTest, verify_message_modified_certificate_signature)
{
    // prepare decap request
    auto secured_message = create_secured_message();
    DecapRequest decap_request(secured_message);

    // iterate through all header_fields
    auto& header_fields = secured_message.header_fields;
    for (auto& field : header_fields) {
        // modify certificate
        if (HeaderFieldType::Signer_Info == get_type(field)) {
            SignerInfo& signer_info = boost::get<SignerInfo>(field);
            Certificate& certificate = boost::get<Certificate>(signer_info);
            certificate.signature = create_random_ecdsa_signature(0);
        }
    }

    // verify message
    DecapConfirm decap_confirm = sec_ent.decapsulate_packet(decap_request);
    ASSERT_FALSE(decap_confirm.certificate_validity);
    EXPECT_EQ(CertificateInvalidReason::INVALID_SIGNATURE, decap_confirm.certificate_validity.reason());
}

TEST_F(SecurityEntityTest, verify_message_modified_signature)
{
    // prepare decap request
    auto secured_message = create_secured_message();
    DecapRequest decap_request(secured_message);

    // iterate through all trailer_fields
    auto& trailer_fields = secured_message.trailer_fields;
    for (auto& field : trailer_fields) {
        // modify signature
        if (TrailerFieldType::Signature == get_type(field)) {
            ASSERT_EQ(PublicKeyAlgorithm::Ecdsa_Nistp256_With_Sha256, get_type(boost::get<Signature>(field)));
            EcdsaSignature& signature = boost::get<EcdsaSignature>(boost::get<Signature>(field));
            signature.s = {8, 15, 23};
        }
    }

    // verify message
    DecapConfirm decap_confirm = sec_ent.decapsulate_packet(decap_request);
    // check if verify was successful
    EXPECT_EQ(ReportType::False_Signature, decap_confirm.report);
}

TEST_F(SecurityEntityTest, verify_message_modified_payload_type)
{
    // prepare decap request
    auto secured_message = create_secured_message();
    DecapRequest decap_request(secured_message);

    // change the payload.type
    secured_message.payload.type = PayloadType::Unsecured;

    // verify message
    DecapConfirm decap_confirm = sec_ent.decapsulate_packet(decap_request);
    // check if verify was successful
    EXPECT_EQ(ReportType::Unsigned_Message, decap_confirm.report);
}

TEST_F(SecurityEntityTest, verify_message_modified_payload)
{
    // prepare decap request
    auto secured_message = create_secured_message();
    DecapRequest decap_request(secured_message);

    // modify payload buffer
    secured_message.payload.data = CohesivePacket({42, 42, 42}, OsiLayer::Session);

    // verify message
    DecapConfirm decap_confirm = sec_ent.decapsulate_packet(decap_request);
    // check if verify was successful
    EXPECT_EQ(ReportType::False_Signature, decap_confirm.report);
}

TEST_F(SecurityEntityTest, verify_message_modified_generation_time_before_current_time)
{
    // change the time, so the generation time of SecuredMessage is before current time
    time_now += std::chrono::hours(12);

    // prepare decap request
    auto secured_message = create_secured_message();
    DecapRequest decap_request(secured_message);

    // change the time, so the current time is before generation time of SecuredMessage
    time_now -= std::chrono::hours(12);

    // verify message
    DecapConfirm decap_confirm = sec_ent.decapsulate_packet(decap_request);
    // check if verify was successful
    EXPECT_EQ(ReportType::Invalid_Timestamp, decap_confirm.report);
}

TEST_F(SecurityEntityTest, verify_message_without_signer_info)
{
    // prepare decap request
    auto secured_message = create_secured_message();
    DecapRequest decap_request(secured_message);

    // iterate through all header_fields
    auto& header_fields = secured_message.header_fields;
    for (auto field = header_fields.begin(); field != header_fields.end(); ++field) {
        // modify certificate
        if (HeaderFieldType::Signer_Info == get_type(*field)) {
            header_fields.erase(field);
            break;
        }
    }

    // verify message
    DecapConfirm decap_confirm = sec_ent.decapsulate_packet(decap_request);
    // check if verify was successful
    EXPECT_EQ(ReportType::Signer_Certificate_Not_Found, decap_confirm.report);
}

// TODO add tests for Unsupported_Signer_Identifier_Type, Incompatible_Protocol
