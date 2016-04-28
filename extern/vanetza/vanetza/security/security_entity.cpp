#include <vanetza/security/security_entity.hpp>
#include <vanetza/security/certificate_manager.hpp>

namespace vanetza
{
namespace security
{

SecurityEntity::SecurityEntity(const Clock::time_point& time_now) :
    m_certificate_manager(new CertificateManager(time_now))
{
}

SecurityEntity::SecurityEntity(std::shared_ptr<CertificateManager> cert_manager) :
    m_certificate_manager(cert_manager)
{
}

EncapConfirm SecurityEntity::encapsulate_packet(const EncapRequest& encap_request)
{
    return sign(encap_request);
}

DecapConfirm SecurityEntity::decapsulate_packet(const DecapRequest& decap_request)
{
    return verify(decap_request);
}

EncapConfirm SecurityEntity::sign(const EncapRequest& encap_request)
{
    return m_certificate_manager->sign_message(encap_request);
}

DecapConfirm SecurityEntity::verify(const DecapRequest& decap_request)
{
    return m_certificate_manager->verify_message(decap_request);
}

} // namespace security
} // namespace vanetza
