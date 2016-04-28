#include <gtest/gtest.h>
#include <vanetza/common/byte_sequence.hpp>
#include <vanetza/security/tests/check_list.hpp>
#include <vanetza/security/tests/check_public_key.hpp>
#include <vanetza/security/tests/check_subject_attribute.hpp>
#include <vanetza/security/tests/check_visitor.hpp>

namespace vanetza
{
namespace security
{

void check(const VerificationKey& expected, const VerificationKey& actual)
{
    SCOPED_TRACE("VerificationKey");
    check(expected.key, actual.key);
}

void check(const EncryptionKey& expected, const EncryptionKey& actual)
{
    SCOPED_TRACE("EncryptionKey");
    check(expected.key, actual.key);
}

void check(const SubjectAssurance& expected, const SubjectAssurance& actual)
{
    EXPECT_EQ(expected.raw, actual.raw);
}

void check(const ItsAidSsp& expected, const ItsAidSsp& actual)
{
    SCOPED_TRACE("ItsAidSsp");
    EXPECT_EQ(expected.its_aid, actual.its_aid);
    EXPECT_EQ(expected.service_specific_permissions, actual.service_specific_permissions);
}

void check(const SubjectAttribute& expected, const SubjectAttribute& actual)
{
    ASSERT_EQ(get_type(expected), get_type(actual));
    boost::apply_visitor(check_visitor<SubjectAttribute>(), expected, actual);
}

VerificationKey create_random_verification_key(int seed)
{
    VerificationKey key;
    key.key = create_random_public_key(seed);
    return key;
}

EncryptionKey create_random_encryption_key(int seed)
{
    EncryptionKey key;
    key.key = create_random_public_key(seed);
    return key;
}

IntX create_random_its_aid(int seed)
{
    IntX result;
    result.set((seed ^ (seed >> 23)) & 0xffffff);
    return result;
}

ItsAidSsp create_random_its_aid_ssp(int seed)
{
    ItsAidSsp result;
    result.its_aid.set(~seed & 0xffffff);
    result.service_specific_permissions = random_byte_sequence(10, seed);
    return result;
}

} // namespace security
} // namespace vanetza
