#ifndef SECURITY_CONTEXT_HPP_FEYZW1RS
#define SECURITY_CONTEXT_HPP_FEYZW1RS

#include <vanetza/common/runtime.hpp>
#include <vanetza/security/backend.hpp>
#include <vanetza/security/naive_certificate_manager.hpp>
#include <vanetza/security/security_entity.hpp>

namespace vanetza
{

class SecurityContext
{
public:
    SecurityContext(Runtime& rt) :
        backend(security::create_backend("default")),
        certificates(new security::NaiveCertificateManager(rt.now())),
        security(
            straight_sign_service(rt, *certificates, *backend),
            straight_verify_service(rt, *certificates, *backend))
    {
    }

    security::SecurityEntity& entity()
    {
        return security;
    }

private:
    std::unique_ptr<security::Backend> backend;
    std::unique_ptr<security::CertificateManager> certificates;
    security::SecurityEntity security;
};

} // namespace vanetza

#endif /* SECURITY_CONTEXT_HPP_FEYZW1RS */

