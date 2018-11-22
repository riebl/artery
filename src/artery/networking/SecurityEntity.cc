#include "artery/networking/Runtime.h"
#include "artery/networking/SecurityEntity.h"
#include "artery/utility/PointerCheck.h"
#include <inet/common/ModuleAccess.h>
#include <vanetza/common/position_provider.hpp>
#include <vanetza/common/runtime.hpp>
#include <vanetza/security/delegating_security_entity.hpp>
#include <vanetza/security/naive_certificate_provider.hpp>
#include <vanetza/security/null_certificate_provider.hpp>
#include <vanetza/security/null_certificate_validator.hpp>

namespace vs = vanetza::security;

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
        mCertificateCache.reset(new vs::CertificateCache(*notNullPtr(mRuntime)));
        mSignHeaderPolicy.reset(new vs::DefaultSignHeaderPolicy(*notNullPtr(mRuntime), *mPositionProvider));
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

std::unique_ptr<vs::CertificateProvider> SecurityEntity::createCertificateProvider(const std::string& name) const
{
    std::unique_ptr<vs::CertificateProvider> certificates;
    if (name == "Null") {
        certificates.reset(new vs::NullCertificateProvider());
    } else if (name == "Naive") {
        certificates.reset(new vs::NaiveCertificateProvider(*notNullPtr(mRuntime)));
    } else {
        error("No certificate provider available with name \"%s\"", name.c_str());
    }
    return certificates;
}

std::unique_ptr<vs::CertificateValidator> SecurityEntity::createCertificateValidator(const std::string& name) const
{
    std::unique_ptr<vs::NullCertificateValidator> validator { new vs::NullCertificateValidator() };

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

vs::SignService SecurityEntity::createSignService(const std::string& name) const
{
    vs::SignService sign_service;

    if (name == "straight") {
        sign_service = vs::straight_sign_service(*mCertificateProvider, *mBackend, *mSignHeaderPolicy);
    } else if (name == "deferred") {
        sign_service = vs::deferred_sign_service(*mCertificateProvider, *mBackend, *mSignHeaderPolicy);
    } else if (name == "dummy") {
        sign_service = vs::dummy_sign_service(*mRuntime, vs::NullCertificateProvider::null_certificate());
    } else {
        error("No security sign service available with name \"%s\"", name.c_str());
    }

    return sign_service;
}

vs::VerifyService SecurityEntity::createVerifyService(const std::string& name) const
{
    vs::VerifyService verify_service;

    if (name == "straight") {
        verify_service = vs::straight_verify_service(*mRuntime, *mCertificateProvider, *mCertificateValidator,
                    *mBackend, *mCertificateCache, *mSignHeaderPolicy, *mPositionProvider);
    } else if (name == "dummy") {
        verify_service = vs::dummy_verify_service(vs::VerificationReport::Success, vs::CertificateValidity::valid());
    } else {
        error("No security verify service available with name \"%s\"", name.c_str());
    }

    return verify_service;
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
