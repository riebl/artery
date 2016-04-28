#ifndef CHECK_RECIPIENT_INFO_HPP_NMX7BFYV
#define CHECK_RECIPIENT_INFO_HPP_NMX7BFYV

#include <vanetza/security/recipient_info.hpp>

namespace vanetza
{
namespace security
{

void check(const EciesEncryptedKey&, const EciesEncryptedKey&);
void check(const OpaqueKey&, const OpaqueKey&);
void check(const Key&, const Key&);
void check(const RecipientInfo&, const RecipientInfo&);

} // namespace security
} // namespace vanetza

#endif /* CHECK_RECIPIENT_INFO_HPP_NMX7BFYV */

