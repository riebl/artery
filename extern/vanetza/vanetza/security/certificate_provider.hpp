#ifndef CERTIFICATE_PROVIDER_HPP
#define CERTIFICATE_PROVIDER_HPP

#include <vanetza/security/certificate.hpp>
#include <vanetza/security/ecdsa256.hpp>

namespace vanetza
{
namespace security
{

class CertificateProvider
{
public:
    /**
     * Get own certificate to use for signing
     * \return own certificate
     */
    virtual const Certificate& own_certificate() = 0;

    /**
     * Get private key associated with own certificate
     * \return private key
     */
    virtual const ecdsa256::PrivateKey& own_private_key() = 0;
};

} // namespace security
} // namespace vanetza

#endif // CERTIFICATE_PROVIDER_HPP
