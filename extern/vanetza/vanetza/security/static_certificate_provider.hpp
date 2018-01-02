#ifndef STATIC_CERTIFICATE_PROVIDER_HPP_MTULFLKX
#define STATIC_CERTIFICATE_PROVIDER_HPP_MTULFLKX

#include <vanetza/security/certificate_provider.hpp>

namespace vanetza
{
namespace security
{

/**
 * \brief A simple certificate provider
 *
 * This certificate provider uses a static certificate and key pair that is pre-generated.
 */
class StaticCertificateProvider : public CertificateProvider
{
public:
    StaticCertificateProvider(const Certificate& authorization_ticket, const ecdsa256::KeyPair& authorization_ticket_key);

    /**
     * Get own certificate to use for signing
     * \return own certificate
     */
    virtual const Certificate& own_certificate() override;

    /**
     * Get private key associated with own certificate
     * \return private key
     */
    virtual const ecdsa256::PrivateKey& own_private_key() override;

private:
    Certificate authorization_ticket;
    ecdsa256::KeyPair authorization_ticket_key;
};

} // namespace security
} // namespace vanetza

#endif /* STATIC_CERTIFICATE_PROVIDER_HPP_MTULFLKX */
