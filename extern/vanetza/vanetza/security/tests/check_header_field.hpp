#ifndef CHECK_HEADER_FIELD_HPP_DNSZT9E2
#define CHECK_HEADER_FIELD_HPP_DNSZT9E2

#include <vanetza/security/header_field.hpp>
#include <vanetza/security/tests/check_basic_elements.hpp>
#include <vanetza/security/tests/check_encryption_parameter.hpp>
#include <vanetza/security/tests/check_list.hpp>
#include <vanetza/security/tests/check_recipient_info.hpp>
#include <vanetza/security/tests/check_region.hpp>
#include <vanetza/security/tests/check_signer_info.hpp>

namespace vanetza
{
namespace security
{

void check(const HeaderField&, const HeaderField&);

} // namespace security
} // namespace vanetza

#endif /* CHECK_HEADER_FIELD_HPP_DNSZT9E2 */

