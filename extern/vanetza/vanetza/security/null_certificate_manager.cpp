#include <vanetza/security/null_certificate_manager.hpp>

namespace vanetza
{
namespace security
{

VerificationKey create_null_verification_key()
{
    Uncompressed coordinates;
    coordinates.x.resize(32);
    coordinates.y.resize(32);
    EccPoint ecc_point = coordinates;
    ecdsa_nistp256_with_sha256 ecdsa;
    ecdsa.public_key = ecc_point;
    VerificationKey verification_key;
    verification_key.key = ecdsa;
    return verification_key;
}

EcdsaSignature create_null_signature()
{
    EcdsaSignature signature;
    X_Coordinate_Only coordinate;
    coordinate.x.resize(32);
    signature.R = std::move(coordinate);
    signature.s.resize(32);
    return signature;
}

Certificate create_null_certificate()
{
    Certificate cert;
    cert.signer_info = HashedId8 {};
    cert.subject_info.subject_type = SubjectType::Authorization_Ticket;
    cert.subject_attributes.push_back(SubjectAssurance(0x00));
    cert.subject_attributes.push_back(create_null_verification_key());
    cert.validity_restriction.push_back(StartAndEndValidity {});
    cert.signature = create_null_signature();
    return cert;
}

NullCertificateManager::NullCertificateManager() : m_check_result(CertificateInvalidReason::INVALID_SIGNATURE)
{
}

CertificateValidity NullCertificateManager::check_certificate(const Certificate&)
{
    return m_check_result;
}

const Certificate& NullCertificateManager::own_certificate()
{
    return null_certificate();
}

const ecdsa256::PrivateKey& NullCertificateManager::own_private_key()
{
    static const ecdsa256::PrivateKey null_key {};
    return null_key;
}

void NullCertificateManager::certificate_check_result(const CertificateValidity& result)
{
    m_check_result = result;
}

const Certificate& NullCertificateManager::null_certificate()
{
    static const Certificate null_certificate = create_null_certificate();
    return null_certificate;
}

} // namespace security
} // namespace vanetza
