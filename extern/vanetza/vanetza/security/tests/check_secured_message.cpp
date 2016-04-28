#include <gtest/gtest.h>
#include <vanetza/security/tests/check_header_field.hpp>
#include <vanetza/security/tests/check_list.hpp>
#include <vanetza/security/tests/check_payload.hpp>
#include <vanetza/security/tests/check_secured_message.hpp>
#include <vanetza/security/tests/check_trailer_field.hpp>

namespace vanetza
{
namespace security
{

void check(const SecuredMessageV2& expected, const SecuredMessageV2& actual)
{
    SCOPED_TRACE("SecuredMessageV2");
    check(expected.header_fields, actual.header_fields);
    check(expected.trailer_fields, actual.trailer_fields);
    check(expected.payload, actual.payload);
}

} // namespace security
} // namespace vanetza
