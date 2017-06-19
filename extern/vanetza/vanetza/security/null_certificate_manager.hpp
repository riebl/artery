#ifndef NULL_CERTIFICATE_MANAGER_HPP_3L9RJY2A
#define NULL_CERTIFICATE_MANAGER_HPP_3L9RJY2A

#include <vanetza/security/certificate_manager.hpp>

namespace vanetza
{
namespace security
{

class NullCertificateManager : public CertificateManager
{
public:
    NullCertificateManager();

    CertificateValidity check_certificate(const Certificate&) override;
    const Certificate& own_certificate() override;
    const ecdsa256::PrivateKey& own_private_key() override;

    /**
     * Set predefined result of check_certificate() calls
     * \param result predefined result
     */
    void certificate_check_result(const CertificateValidity& result);

    /**
     * Get static dummy certificate
     * \return certificate filled with dummy values
     */
    static const Certificate& null_certificate();

private:
    CertificateValidity m_check_result;
};

} // namespace security
} // namespace vanetza

#endif /* NULL_CERTIFICATE_MANAGER_HPP_3L9RJY2A */

