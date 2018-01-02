#ifndef CERTIFICATE_VALIDATOR_HPP
#define CERTIFICATE_VALIDATOR_HPP

#include <vanetza/security/certificate.hpp>

namespace vanetza
{
namespace security
{

class CertificateValidator
{
public:
    /**
     * Check validity of given certificate
     * \param certificate given certificate
     * \return validity result
     */
    virtual CertificateValidity check_certificate(const Certificate& certificate) = 0;
};

} // namespace security
} // namespace vanetza

#endif // CERTIFICATE_VALIDATOR_HPP
