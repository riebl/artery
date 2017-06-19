#include <vanetza/common/clock.hpp>
#include <vanetza/security/naive_certificate_manager.hpp>
#include <boost/variant/get.hpp>
#include <gtest/gtest.h>

using namespace vanetza;
using namespace vanetza::security;
using boost::get;

class CertificateManagerTest : public ::testing::Test
{
public:
    CertificateManagerTest() : time_now(Clock::at("2016-08-01 00:00")), cert_manager(time_now)
    {
    }

protected:
    Clock::time_point time_now;
    NaiveCertificateManager cert_manager;
};

TEST_F(CertificateManagerTest, own_certificate)
{
    Certificate signed_certificate = cert_manager.own_certificate();

    // Check signature
    EXPECT_EQ(2 * field_size(PublicKeyAlgorithm::Ecdsa_Nistp256_With_Sha256),
              extract_signature_buffer(signed_certificate.signature).size());
    EXPECT_EQ(PublicKeyAlgorithm::Ecdsa_Nistp256_With_Sha256, get_type(signed_certificate.signature));

    // Check signer_info and subject_info
    EXPECT_EQ(2, signed_certificate.version());
    EXPECT_EQ(SignerInfoType::Certificate_Digest_With_SHA256, get_type(signed_certificate.signer_info));
    EXPECT_EQ(SubjectType::Authorization_Ticket, signed_certificate.subject_info.subject_type);
    EXPECT_TRUE(signed_certificate.subject_info.subject_name.empty());

    // Check subject attributes
    int verification_key_counter = 0;
    SubjectAssurance test_assurance_level;
    int assurance_level_counter = 0;

    for (auto& subject_attribute : signed_certificate.subject_attributes) {

        if (SubjectAttributeType::Verification_Key == get_type(subject_attribute)) {
            verification_key_counter++;
        } else if (SubjectAttributeType::Assurance_Level == get_type(subject_attribute)) {
            test_assurance_level = get<SubjectAssurance>(subject_attribute);
            assurance_level_counter++;
        } else if (SubjectAttributeType::Its_Aid_Ssp_List == get_type(subject_attribute)) {
            // TODO: check aid permissions
        }
    }
    EXPECT_EQ(1, verification_key_counter);
    ASSERT_EQ(1, assurance_level_counter);
    EXPECT_EQ(0, test_assurance_level.raw);

    // Check validity restrictions
    Time32 start_time;
    Time32 end_time;
    for (ValidityRestriction restriction : signed_certificate.validity_restriction){
        if (ValidityRestrictionType::Time_Start_And_End == get_type(restriction)) {
            StartAndEndValidity& time_validation = get<StartAndEndValidity>(restriction);
            start_time = time_validation.start_validity;
            end_time = time_validation.end_validity;
        } else if (ValidityRestrictionType::Region == get_type(restriction)) {
            // TODO: Region not specified yet
        }
    }
    EXPECT_LT(start_time, end_time);
}

