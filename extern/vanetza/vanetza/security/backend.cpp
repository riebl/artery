#include <vanetza/common/factory.hpp>
#include <vanetza/security/backend.hpp>
#include <cassert>

namespace
{

static vanetza::Factory<vanetza::security::Backend> backend_factory;

template<typename T>
class BackendRegistrar
{
public:
    BackendRegistrar(const std::string& name)
    {
        auto f = []() { return std::unique_ptr<vanetza::security::Backend> { new T() }; };
        bool success = backend_factory.add(name, f) && backend_factory.configure_default(name);
        assert(success);
    }
};

} // namespace

// order of VANETZA_REGISTER_CRYPTO_BACKEND invocation defines priority: last registered one will be default
#define VANETZA_REGISTER_CRYPTO_BACKEND(clazz) \
    namespace { \
        using namespace vanetza::security; \
        static const BackendRegistrar<clazz> clazz##Factory(clazz::backend_name); \
    }

#include <vanetza/security/backend_null.hpp>
VANETZA_REGISTER_CRYPTO_BACKEND(BackendNull)

#ifdef VANETZA_WITH_CRYPTOPP
#   include <vanetza/security/backend_cryptopp.hpp>
    VANETZA_REGISTER_CRYPTO_BACKEND(BackendCryptoPP)
#endif

#ifdef VANETZA_WITH_OPENSSL
#   include <vanetza/security/backend_openssl.hpp>
    VANETZA_REGISTER_CRYPTO_BACKEND(BackendOpenSsl)
#endif

namespace vanetza
{
namespace security
{

const Factory<Backend>& builtin_backends()
{
    return backend_factory;
}

std::unique_ptr<Backend> create_backend(const std::string& name, const Factory<Backend>& factory)
{
    if (name == "default") {
        return factory.create();
    } else {
        return factory.create(name);
    }
}

} // namespace security
} // namespace vanetza
