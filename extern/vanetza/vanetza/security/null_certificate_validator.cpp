#include <vanetza/security/null_certificate_validator.hpp>

namespace vanetza
{
namespace security
{

NullCertificateValidator::NullCertificateValidator() : m_check_result(CertificateInvalidReason::UNKNOWN_SIGNER)
{
}

CertificateValidity NullCertificateValidator::check_certificate(const Certificate&)
{
    return m_check_result;
}

void NullCertificateValidator::certificate_check_result(const CertificateValidity& result)
{
    m_check_result = result;
}

} // namespace security
} // namespace vanetza
