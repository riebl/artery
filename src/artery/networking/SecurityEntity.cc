#include "artery/networking/Runtime.h"
#include "artery/networking/SecurityEntity.h"
#include "artery/utility/PointerCheck.h"
#include <inet/common/ModuleAccess.h>
#include <vanetza/common/position_provider.hpp>
#include <vanetza/common/runtime.hpp>
#include <vanetza/security/delegating_security_entity.hpp>
#include <vanetza/security/straight_verify_service.hpp>
#include <vanetza/security/v2/naive_certificate_provider.hpp>
#include <vanetza/security/v2/null_certificate_provider.hpp>
#include <vanetza/security/v2/null_certificate_validator.hpp>
#include <vanetza/security/v2/sign_service.hpp>

namespace vs = vanetza::security;
namespace vs2 = vanetza::security::v2;

namespace artery
{

Define_Module(SecurityEntity)

int SecurityEntity::numInitStages() const
{
    return 2;
}

void SecurityEntity::initialize(int stage)
{
    if (stage == 0) {
        mRuntime = inet::findModuleFromPar<Runtime>(par("runtimeModule"), this);
        mPositionProvider = inet::findModuleFromPar<vanetza::PositionProvider>(par("positionModule"), this);
    } else if (stage == 1){
        mBackend = createBackend(par("CryptoBackend"));
        mCertificateProvider = createCertificateProvider(par("CertificateProvider"));
        mCertificateValidator = createCertificateValidator(par("CertificateValidator"));
        mCertificateCache.reset(new vs2::CertificateCache(*notNullPtr(mRuntime)));
        mSignHeaderPolicy.reset(new vs2::DefaultSignHeaderPolicy(*notNullPtr(mRuntime), *mPositionProvider));
        mEntity.reset(new vs::DelegatingSecurityEntity(createSignService(par("SignService")), createVerifyService(par("VerifyService"))));
    }
}

void SecurityEntity::finish()
{
    // free objects before runtime vanishes
    mEntity.reset();
    mSignHeaderPolicy.reset();
    mCertificateCache.reset();
    mCertificateValidator.reset();
    mCertificateProvider.reset();
    mBackend.reset();
}

std::unique_ptr<vs::Backend> SecurityEntity::createBackend(const std::string& name) const
{
    auto backend = vs::create_backend(name.c_str());
    if (!backend) {
        error("No security backend found with name \"%s\"", name.c_str());
    }
    return backend;
}

std::unique_ptr<vs2::CertificateProvider> SecurityEntity::createCertificateProvider(const std::string& name) const
{
    std::unique_ptr<vs2::CertificateProvider> certificates;
    if (name == "Null") {
        certificates.reset(new vs2::NullCertificateProvider());
    } else if (name == "Naive") {
        certificates.reset(new vs2::NaiveCertificateProvider(*notNullPtr(mRuntime)));
    } else {
        error("No certificate provider available with name \"%s\"", name.c_str());
    }
    return certificates;
}

std::unique_ptr<vs2::CertificateValidator> SecurityEntity::createCertificateValidator(const std::string& name) const
{
    std::unique_ptr<vs2::NullCertificateValidator> validator { new vs2::NullCertificateValidator() };

    if (name == "Null") {
        // no-op
    } else if (name == "NullOk") {
        static const vs::CertificateValidity ok;
        ASSERT(ok);
        validator->certificate_check_result(ok);
    } else {
        error("No certificate validator available with name \"%s\"", name.c_str());
    }

    return validator;
}

std::unique_ptr<vs::SignService> SecurityEntity::createSignService(const std::string& name) const
{
    if (name == "straight") {
        return std::make_unique<vs2::StraightSignService>(*mCertificateProvider, *mBackend, *mSignHeaderPolicy);
    } else if (name == "deferred") {
        return std::make_unique<vs2::DeferredSignService>(*mCertificateProvider, *mBackend, *mSignHeaderPolicy);
    } else if (name == "dummy") {
        return std::make_unique<vs2::DummySignService>(*mRuntime, vs2::NullCertificateProvider::null_certificate());
    } else {
        error("No security sign service available with name \"%s\"", name.c_str());
        return nullptr;
    }
}

std::unique_ptr<vs::VerifyService> SecurityEntity::createVerifyService(const std::string& name) const
{
    if (name == "straight") {
        auto verify_service = std::make_unique<vs::StraightVerifyService>(*mRuntime, *mBackend, *mPositionProvider);
        verify_service->use_certificate_cache(mCertificateCache.get());
        verify_service->use_certificate_provider(mCertificateProvider.get());
        verify_service->use_certificate_validator(mCertificateValidator.get());
        verify_service->use_sign_header_policy(mSignHeaderPolicy.get());
        return verify_service;
    } else if (name == "dummy") {
        return std::make_unique<vs::DummyVerifyService>(vs::VerificationReport::Success, vs::CertificateValidity::valid());
    } else {
        error("No security verify service available with name \"%s\"", name.c_str());
        return nullptr;
    }
}

vs::EncapConfirm SecurityEntity::encapsulate_packet(vs::EncapRequest&& request)
{
    return notNullPtr(mEntity)->encapsulate_packet(std::move(request));
}

vs::DecapConfirm SecurityEntity::decapsulate_packet(vs::DecapRequest&& request)
{
    return notNullPtr(mEntity)->decapsulate_packet(std::move(request));
}

} // namespace artery
