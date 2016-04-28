#include <gtest/gtest.h>
#include <vanetza/security/tests/check_encryption_parameter.hpp>
#include <vanetza/security/tests/check_visitor.hpp>

namespace vanetza
{
namespace security
{

void check(const EncryptionParameter& expected, const EncryptionParameter& actual)
{
    ASSERT_EQ(get_type(expected), get_type(actual));
    EXPECT_EQ(get_size(expected), get_size(actual));
    boost::apply_visitor(check_visitor<EncryptionParameter>(), expected, actual);
}

} // namespace security
} // namespace vanetza
