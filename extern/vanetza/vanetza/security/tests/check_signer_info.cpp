#include <gtest/gtest.h>
#include <vanetza/security/tests/check_basic_elements.hpp>
#include <vanetza/security/tests/check_certificate.hpp>
#include <vanetza/security/tests/check_list.hpp>
#include <vanetza/security/tests/check_signer_info.hpp>
#include <vanetza/security/tests/check_visitor.hpp>

namespace vanetza
{
namespace security
{

void check(const std::nullptr_t&, const std::nullptr_t&)
{
}

void check(const CertificateDigestWithOtherAlgorithm& expected, const CertificateDigestWithOtherAlgorithm& actual)
{
    EXPECT_EQ(expected.algorithm, actual.algorithm);
    EXPECT_EQ(expected.digest, actual.digest);
}

void check(const boost::recursive_wrapper<Certificate>& expected, const boost::recursive_wrapper<Certificate>& actual)
{
    check(expected.get(), actual.get());
}

void check(const SignerInfo& expected, const SignerInfo& actual)
{
    ASSERT_EQ(get_type(expected), get_type(actual));
    boost::apply_visitor(check_visitor<SignerInfo>(), expected, actual);
}

} // namespace security
} // namespace vanetza
