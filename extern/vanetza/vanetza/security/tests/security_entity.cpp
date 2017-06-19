#include <gtest/gtest.h>
#include <vanetza/common/runtime.hpp>
#include <vanetza/security/backend.hpp>
#include <vanetza/security/naive_certificate_manager.hpp>
#include <vanetza/security/security_entity.hpp>
#include <vanetza/security/tests/check_payload.hpp>
#include <vanetza/security/tests/check_signature.hpp>

using namespace vanetza;
using namespace vanetza::security;

class SecurityEntityTest : public ::testing::Test
{
protected:
    SecurityEntityTest() :
        crypto_backend(create_backend("default")),
        certificate_manager(new NaiveCertificateManager(runtime.now())),
        sign_service(straight_sign_service(runtime, *certificate_manager, *crypto_backend)),
        verify_service(straight_verify_service(runtime, *certificate_manager, *crypto_backend)),
        security(sign_service, verify_service)
    {
    }

    void SetUp() override
    {
        runtime.reset(Clock::at("2016-03-7 08:23"));
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
        EncapConfirm confirm = security.encapsulate_packet(create_encap_request());
        return confirm.sec_packet;
    }

    Runtime runtime;
    std::unique_ptr<Backend> crypto_backend;
    std::unique_ptr<CertificateManager> certificate_manager;
    SignService sign_service;
    VerifyService verify_service;
    SecurityEntity security;
    ChunkPacket expected_payload;
};

TEST_F(SecurityEntityTest, mutual_acceptance)
{
    SignService sign = straight_sign_service(runtime, *certificate_manager, *crypto_backend);
    VerifyService verify = straight_verify_service(runtime, *certificate_manager, *crypto_backend);
    SecurityEntity other_security(sign, verify);
    EncapConfirm encap_confirm = other_security.encapsulate_packet(create_encap_request());
    DecapConfirm decap_confirm = security.decapsulate_packet(DecapRequest { encap_confirm.sec_packet });
    EXPECT_EQ(DecapReport::Success, decap_confirm.report);
}

#if defined(VANETZA_WITH_CRYPTOPP) && defined(VANETZA_WITH_OPENSSL)
TEST_F(SecurityEntityTest, mutual_acceptance_impl)
{
    NaiveCertificateManager cryptopp_certs(runtime.now()), openssl_certs(runtime.now());
    auto cryptopp_backend = create_backend("CryptoPP");
    auto openssl_backend = create_backend("OpenSSL");
    ASSERT_TRUE(cryptopp_backend);
    ASSERT_TRUE(openssl_backend);
    SecurityEntity cryptopp_security {
            straight_sign_service(runtime, cryptopp_certs, *cryptopp_backend),
            straight_verify_service(runtime, cryptopp_certs, *cryptopp_backend) };
    SecurityEntity openssl_security {
            straight_sign_service(runtime, openssl_certs, *openssl_backend),
            straight_verify_service(runtime, openssl_certs, *openssl_backend) };

    // OpenSSL to Crypto++
    EncapConfirm encap_confirm = openssl_security.encapsulate_packet(create_encap_request());
    DecapConfirm decap_confirm = cryptopp_security.decapsulate_packet(DecapRequest { encap_confirm.sec_packet });
    EXPECT_EQ(DecapReport::Success, decap_confirm.report);

    // Crypto++ to OpenSSL
    encap_confirm = cryptopp_security.encapsulate_packet(create_encap_request());
    decap_confirm = openssl_security.decapsulate_packet(DecapRequest { encap_confirm.sec_packet });
    EXPECT_EQ(DecapReport::Success, decap_confirm.report);
}
#endif

TEST_F(SecurityEntityTest, signed_payload_equals_plaintext_payload)
{
    EncapConfirm confirm = security.encapsulate_packet(create_encap_request());

    // check if sec_payload equals plaintext_payload
    check(expected_payload, confirm.sec_packet.payload.data);
}

TEST_F(SecurityEntityTest, signature_is_ecdsa)
{
    EncapConfirm confirm = security.encapsulate_packet(create_encap_request());

    // check if trailer_fields contain signature
    EXPECT_EQ(1, confirm.sec_packet.trailer_fields.size());
    auto signature = confirm.sec_packet.trailer_field(TrailerFieldType::Signature);
    ASSERT_TRUE(!!signature);
    auto signature_type = get_type(boost::get<Signature>(*signature));
    EXPECT_EQ(PublicKeyAlgorithm::Ecdsa_Nistp256_With_Sha256, signature_type);
}

TEST_F(SecurityEntityTest, expected_header_field_size)
{
    EncapConfirm confirm = security.encapsulate_packet(create_encap_request());

    // check header_field size
    EXPECT_EQ(3, confirm.sec_packet.header_fields.size());
}

TEST_F(SecurityEntityTest, expected_payload)
{
    EncapConfirm confirm = security.encapsulate_packet(create_encap_request());

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
    DecapConfirm decap_confirm = security.decapsulate_packet(std::move(decap_request));

    // check if verify was successful
    EXPECT_EQ(DecapReport::Success, decap_confirm.report);
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

    IntX* its_aid = secured_message.header_field<HeaderFieldType::Its_Aid>();
    ASSERT_TRUE(its_aid);
    its_aid->set(42);

    // verify message
    DecapConfirm decap_confirm = security.decapsulate_packet(std::move(decap_request));
    // check if verify was successful
    EXPECT_EQ(DecapReport::False_Signature, decap_confirm.report);
}

TEST_F(SecurityEntityTest, verify_message_modified_certificate_name)
{
    // create decap request
    auto secured_message = create_secured_message();
    DecapRequest decap_request(secured_message);

    // change the subject name
    SignerInfo* signer_info = secured_message.header_field<HeaderFieldType::Signer_Info>();
    ASSERT_TRUE(signer_info);
    Certificate& certificate = boost::get<Certificate>(*signer_info);
    certificate.subject_info.subject_name = {42};

    // verify message
    DecapConfirm decap_confirm = security.decapsulate_packet(std::move(decap_request));
    ASSERT_FALSE(decap_confirm.certificate_validity);
    EXPECT_EQ(CertificateInvalidReason::INVALID_NAME, decap_confirm.certificate_validity.reason());
}

TEST_F(SecurityEntityTest, verify_message_modified_certificate_signer_info)
{
    // create decap request
    auto secured_message = create_secured_message();
    DecapRequest decap_request(secured_message);

    // change the subject info
    SignerInfo* signer_info = secured_message.header_field<HeaderFieldType::Signer_Info>();
    ASSERT_TRUE(signer_info);
    Certificate& certificate = boost::get<Certificate>(*signer_info);
    HashedId8 faulty_hash {{ 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 }};
    certificate.signer_info = faulty_hash;

    // verify message
    DecapConfirm decap_confirm = security.decapsulate_packet(std::move(decap_request));
    ASSERT_FALSE(decap_confirm.certificate_validity);
    EXPECT_EQ(CertificateInvalidReason::INVALID_ROOT_HASH, decap_confirm.certificate_validity.reason());
}

TEST_F(SecurityEntityTest, verify_message_modified_certificate_subject_info)
{
    // create decap request
    auto secured_message = create_secured_message();
    DecapRequest decap_request(secured_message);

    // change the subject info
    SignerInfo* signer_info = secured_message.header_field<HeaderFieldType::Signer_Info>();
    ASSERT_TRUE(signer_info);
    Certificate& certificate = boost::get<Certificate>(*signer_info);
    certificate.subject_info.subject_type = SubjectType::Root_Ca;

    // verify message
    DecapConfirm decap_confirm = security.decapsulate_packet(std::move(decap_request));
    ASSERT_FALSE(decap_confirm.certificate_validity);
    EXPECT_EQ(CertificateInvalidReason::INVALID_SIGNATURE, decap_confirm.certificate_validity.reason());
}

TEST_F(SecurityEntityTest, verify_message_modified_certificate_subject_assurance)
{
    // create decap request
    auto secured_message = create_secured_message();
    DecapRequest decap_request(secured_message);

    SignerInfo* signer_info = secured_message.header_field<HeaderFieldType::Signer_Info>();
    ASSERT_TRUE(signer_info);
    Certificate& certificate = boost::get<Certificate>(*signer_info);
    for (auto& subject_attribute : certificate.subject_attributes) {
        if (SubjectAttributeType::Assurance_Level == get_type(subject_attribute)) {
            SubjectAssurance& subject_assurance = boost::get<SubjectAssurance>(subject_attribute);
            // change raw in subject assurance
            subject_assurance.raw = 0x47;
        }
    }

    // verify message
    DecapConfirm decap_confirm = security.decapsulate_packet(std::move(decap_request));
    ASSERT_FALSE(decap_confirm.certificate_validity);
    EXPECT_EQ(CertificateInvalidReason::INVALID_SIGNATURE, decap_confirm.certificate_validity.reason());
}

TEST_F(SecurityEntityTest, verify_message_outdated_certificate)
{
    // prepare decap request
    auto secured_message = create_secured_message();

    // forge certificate with outdatet validity
    StartAndEndValidity outdated_validity;
    outdated_validity.start_validity = convert_time32(runtime.now() - std::chrono::hours(1));
    outdated_validity.end_validity = convert_time32(runtime.now() - std::chrono::minutes(1));
    auto signer_info_field = secured_message.header_field(HeaderFieldType::Signer_Info);
    ASSERT_TRUE(signer_info_field);
    auto certificate = boost::get<Certificate>(&boost::get<SignerInfo>(*signer_info_field));
    ASSERT_TRUE(certificate);
    certificate->validity_restriction.clear();
    certificate->validity_restriction.push_back(outdated_validity);

    // verify message
    DecapConfirm decap_confirm = security.decapsulate_packet(DecapRequest { secured_message });
    EXPECT_EQ(DecapReport::Invalid_Certificate, decap_confirm.report);
    ASSERT_FALSE(decap_confirm.certificate_validity);
    EXPECT_EQ(CertificateInvalidReason::OFF_TIME_PERIOD, decap_confirm.certificate_validity.reason());
}

TEST_F(SecurityEntityTest, verify_message_premature_certificate)
{
    // prepare decap request
    auto secured_message = create_secured_message();

    // forge certificate with premature validity
    StartAndEndValidity premature_validity;
    premature_validity.start_validity = convert_time32(runtime.now() + std::chrono::hours(1));
    premature_validity.end_validity = convert_time32(runtime.now() + std::chrono::hours(25));
    auto signer_info_field = secured_message.header_field(HeaderFieldType::Signer_Info);
    ASSERT_TRUE(signer_info_field);
    auto certificate = boost::get<Certificate>(&boost::get<SignerInfo>(*signer_info_field));
    ASSERT_TRUE(certificate);
    certificate->validity_restriction.clear();
    certificate->validity_restriction.push_back(premature_validity);

    // verify message
    DecapConfirm decap_confirm = security.decapsulate_packet(DecapRequest { secured_message });
    EXPECT_EQ(DecapReport::Invalid_Certificate, decap_confirm.report);
    ASSERT_FALSE(decap_confirm.certificate_validity);
    EXPECT_EQ(CertificateInvalidReason::OFF_TIME_PERIOD, decap_confirm.certificate_validity.reason());
}

TEST_F(SecurityEntityTest, verify_message_modified_certificate_validity_restriction)
{
    // prepare decap request
    auto secured_message = create_secured_message();
    DecapRequest decap_request(secured_message);

    SignerInfo* signer_info = secured_message.header_field<HeaderFieldType::Signer_Info>();
    ASSERT_TRUE(signer_info);
    Certificate& certificate = boost::get<Certificate>(*signer_info);
    for (auto& validity_restriction : certificate.validity_restriction) {
        ValidityRestrictionType type = get_type(validity_restriction);
        ASSERT_EQ(type, ValidityRestrictionType::Time_Start_And_End);

        // change start and end time of certificate validity
        StartAndEndValidity& start_and_end = boost::get<StartAndEndValidity>(validity_restriction);
        start_and_end.start_validity = 500;
        start_and_end.end_validity = 373;
    }

    // verify message
    DecapConfirm decap_confirm = security.decapsulate_packet(std::move(decap_request));
    ASSERT_FALSE(decap_confirm.certificate_validity);
    EXPECT_EQ(CertificateInvalidReason::BROKEN_TIME_PERIOD, decap_confirm.certificate_validity.reason());
}

TEST_F(SecurityEntityTest, verify_message_modified_certificate_signature)
{
    // prepare decap request
    auto secured_message = create_secured_message();
    DecapRequest decap_request(secured_message);

    SignerInfo* signer_info = secured_message.header_field<HeaderFieldType::Signer_Info>();
    ASSERT_TRUE(signer_info);
    Certificate& certificate = boost::get<Certificate>(*signer_info);
    certificate.signature = create_random_ecdsa_signature(0);

    // verify message
    DecapConfirm decap_confirm = security.decapsulate_packet(std::move(decap_request));
    ASSERT_FALSE(decap_confirm.certificate_validity);
    EXPECT_EQ(CertificateInvalidReason::INVALID_SIGNATURE, decap_confirm.certificate_validity.reason());
}

TEST_F(SecurityEntityTest, verify_message_modified_signature)
{
    // prepare decap request
    auto secured_message = create_secured_message();
    DecapRequest decap_request(secured_message);

    Signature* signature = secured_message.trailer_field<TrailerFieldType::Signature>();
    ASSERT_TRUE(signature);
    ASSERT_EQ(PublicKeyAlgorithm::Ecdsa_Nistp256_With_Sha256, get_type(*signature));
    EcdsaSignature& ecdsa_signature = boost::get<EcdsaSignature>(*signature);
    ecdsa_signature.s = {8, 15, 23};

    // verify message
    DecapConfirm decap_confirm = security.decapsulate_packet(std::move(decap_request));
    // check if verify was successful
    EXPECT_EQ(DecapReport::False_Signature, decap_confirm.report);
}

TEST_F(SecurityEntityTest, verify_message_modified_payload_type)
{
    // prepare decap request
    auto secured_message = create_secured_message();
    DecapRequest decap_request(secured_message);

    // change the payload.type
    secured_message.payload.type = PayloadType::Unsecured;

    // verify message
    DecapConfirm decap_confirm = security.decapsulate_packet(std::move(decap_request));
    // check if verify was successful
    EXPECT_EQ(DecapReport::Unsigned_Message, decap_confirm.report);
}

TEST_F(SecurityEntityTest, verify_message_modified_payload)
{
    // prepare decap request
    auto secured_message = create_secured_message();
    DecapRequest decap_request(secured_message);

    // modify payload buffer
    secured_message.payload.data = CohesivePacket({42, 42, 42}, OsiLayer::Session);

    // verify message
    DecapConfirm decap_confirm = security.decapsulate_packet(std::move(decap_request));
    // check if verify was successful
    EXPECT_EQ(DecapReport::False_Signature, decap_confirm.report);
}

TEST_F(SecurityEntityTest, verify_message_modified_generation_time_before_current_time)
{
    // change the time, so the generation time of SecuredMessage is before current time
    runtime.trigger(std::chrono::hours(12));

    // prepare decap request
    auto secured_message = create_secured_message();
    DecapRequest decap_request(secured_message);

    // change the time, so the current time is before generation time of SecuredMessage
    runtime.reset(runtime.now() - std::chrono::hours(12));

    // verify message
    DecapConfirm decap_confirm = security.decapsulate_packet(std::move(decap_request));
    // check if verify was successful
    EXPECT_EQ(DecapReport::Invalid_Timestamp, decap_confirm.report);
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
    DecapConfirm decap_confirm = security.decapsulate_packet(std::move(decap_request));
    // check if verify was successful
    EXPECT_EQ(DecapReport::Signer_Certificate_Not_Found, decap_confirm.report);
}

// TODO add tests for Unsupported_Signer_Identifier_Type, Incompatible_Protocol
