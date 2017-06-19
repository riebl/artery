#include <vanetza/common/runtime.hpp>
#include <vanetza/security/certificate_manager.hpp>
#include <vanetza/security/naive_certificate_manager.hpp>
#include <vanetza/security/null_certificate_manager.hpp>
#include <cassert>

namespace vanetza
{
namespace security
{
namespace
{

Factory<CertificateManager, Runtime&> setup_factory()
{
    Factory<CertificateManager, Runtime&> factory;
    factory.add("Naive", [](Runtime& rt) {
            return std::unique_ptr<CertificateManager> { new NaiveCertificateManager(rt.now()) };
    });
    factory.add("Null", [](Runtime&) {
            return std::unique_ptr<CertificateManager> { new NullCertificateManager() };
    });
    factory.add("NullOk", [](Runtime&) {
            static const CertificateValidity ok {};
            assert(ok);
            std::unique_ptr<NullCertificateManager> manager { new NullCertificateManager() };
            manager->certificate_check_result(ok);
            return std::unique_ptr<CertificateManager> { std::move(manager) };
    });
    return factory;
}

} // namespace

const Factory<CertificateManager, Runtime&>& builtin_certificate_managers()
{
    static const auto factory = setup_factory();
    return factory;
};

} // namespace security
} // namespace vanetza
