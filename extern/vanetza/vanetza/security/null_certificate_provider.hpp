#ifndef NULL_CERTIFICATE_PROVIDER_HPP_3L9RJY2A
#define NULL_CERTIFICATE_PROVIDER_HPP_3L9RJY2A

#include <vanetza/security/certificate_provider.hpp>

namespace vanetza
{
namespace security
{

class NullCertificateProvider : public CertificateProvider
{
public:
    NullCertificateProvider();

    const Certificate& own_certificate() override;
    const ecdsa256::PrivateKey& own_private_key() override;

    /**
     * Get static dummy certificate
     * \return certificate filled with dummy values
     */
    static const Certificate& null_certificate();
};

} // namespace security
} // namespace vanetza

#endif /* NULL_CERTIFICATE_PROVIDER_HPP_3L9RJY2A */
