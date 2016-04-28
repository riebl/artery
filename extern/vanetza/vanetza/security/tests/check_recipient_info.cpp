#include <gtest/gtest.h>
#include <vanetza/security/tests/check_ecc_point.hpp>
#include <vanetza/security/tests/check_recipient_info.hpp>
#include <vanetza/security/tests/check_visitor.hpp>

namespace vanetza
{
namespace security
{

void check(const RecipientInfo& expected, const RecipientInfo& actual)
{
    EXPECT_EQ(expected.cert_id, actual.cert_id);
    check(expected.enc_key, actual.enc_key);
}

void check(const EciesEncryptedKey& expected, const EciesEncryptedKey& actual)
{
    check(expected.v, actual.v);
    EXPECT_EQ(expected.c, actual.c);
    EXPECT_EQ(expected.t, actual.t);
}

void check(const OpaqueKey& expected, const OpaqueKey& actual)
{
    EXPECT_EQ(expected.data, actual.data);
}

void check(const Key& expected, const Key& actual)
{
    ASSERT_EQ(get_type(expected), get_type(actual));
    boost::apply_visitor(check_visitor<Key>(), expected, actual);
}

} // namespace security
} // namespace vanetza

