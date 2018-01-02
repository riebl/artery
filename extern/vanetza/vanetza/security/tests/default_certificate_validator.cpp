#include <vanetza/common/clock.hpp>
#include <vanetza/security/default_certificate_validator.hpp>
#include <vanetza/security/naive_certificate_provider.hpp>
#include <vanetza/security/trust_store.hpp>
#include <boost/variant/get.hpp>
#include <gtest/gtest.h>

using namespace vanetza;
using namespace vanetza::security;

class DefaultCertificateValidatorTest : public ::testing::Test
{
public:
    DefaultCertificateValidatorTest() :
        time_now(Clock::at("2016-08-01 00:00")),
        cert_provider(time_now),
        roots({ cert_provider.root_certificate() }),
        trust_store(roots),
        cert_validator(time_now, trust_store)
    {
    }

protected:
    Clock::time_point time_now;
    NaiveCertificateProvider cert_provider;
    std::vector<Certificate> roots;
    TrustStore trust_store;
    DefaultCertificateValidator cert_validator;
};

TEST_F(DefaultCertificateValidatorTest, validity_time)
{
    Certificate cert = cert_provider.own_certificate();

    // remove any time constraint from certificate
    for (auto it = cert.validity_restriction.begin(); it != cert.validity_restriction.end(); ++it) {
        const ValidityRestriction& restriction = *it;
        ValidityRestrictionType type = get_type(restriction);
        switch (type) {
            case ValidityRestrictionType::Time_End:
            case ValidityRestrictionType::Time_Start_And_End:
            case ValidityRestrictionType::Time_Start_And_Duration:
                it = cert.validity_restriction.erase(it);
                break;
            default:
                break;
        }
    }

    CertificateValidity validity = cert_validator.check_certificate(cert);
    ASSERT_FALSE(validity);
    EXPECT_EQ(CertificateInvalidReason::BROKEN_TIME_PERIOD, validity.reason());
    // TODO: check that presence of exactly one time constraint is considered valid
}
