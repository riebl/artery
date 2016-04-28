#include <gtest/gtest.h>
#include <vanetza/security/tests/check_certificate.hpp>
#include <vanetza/security/tests/check_list.hpp>
#include <vanetza/security/tests/check_signature.hpp>
#include <vanetza/security/tests/check_signer_info.hpp>
#include <vanetza/security/tests/check_subject_attribute.hpp>
#include <vanetza/security/tests/check_subject_info.hpp>
#include <vanetza/security/tests/check_validity_restriction.hpp>

namespace vanetza
{
namespace security
{

void check(const Certificate& expected, const Certificate& actual)
{
    // certificate version is static, no check required
    check(expected.signer_info, actual.signer_info);
    check(expected.subject_info, actual.subject_info);
    check(expected.subject_attributes, actual.subject_attributes, "SubjectAttribute");
    check(expected.validity_restriction, actual.validity_restriction, "ValidityRestriction");
    check(expected.signature, actual.signature);
}

} // namespace security
} // namespace vanetza
