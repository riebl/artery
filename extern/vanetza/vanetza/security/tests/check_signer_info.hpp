#ifndef CHECK_SIGNER_INFO_HPP_S2AYJSYC
#define CHECK_SIGNER_INFO_HPP_S2AYJSYC

#include <vanetza/security/signer_info.hpp>

namespace vanetza
{
namespace security
{

void check(const std::nullptr_t&, const std::nullptr_t&);
void check(const CertificateDigestWithOtherAlgorithm&, const CertificateDigestWithOtherAlgorithm&);
void check(const boost::recursive_wrapper<Certificate>&, const boost::recursive_wrapper<Certificate>&);
void check(const SignerInfo&, const SignerInfo&);

} // namespace security
} // namespace vanetza

#endif /* CHECK_SIGNER_INFO_HPP_S2AYJSYC */

