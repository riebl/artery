#include <vanetza/security/basic_elements.hpp>
#include <vanetza/security/ecc_point.hpp>
#include <vanetza/security/naive_certificate_manager.hpp>
#include <vanetza/security/payload.hpp>
#include <vanetza/security/secured_message.hpp>
#include <vanetza/security/signature.hpp>
#include <chrono>

namespace vanetza
{
namespace security
{

NaiveCertificateManager::NaiveCertificateManager(const Clock::time_point& time_now) :
    m_time_now(time_now),
    m_root_key_pair(root_key_pair()),
    m_root_certificate_hash(HashedId8 {{ 0x17, 0x5c, 0x33, 0x48, 0x25, 0xdc, 0x7f, 0xab }}),
    m_own_key_pair(m_crypto_backend.generate_key_pair()),
    m_own_certificate(generate_certificate(m_own_key_pair))
{
    // TODO: root certifiate hash is arbitrarily chosen for now (no root certificate exists)
}

const Certificate& NaiveCertificateManager::own_certificate()
{
    // renew certificate if necessary
    for (auto& validity_restriction : m_own_certificate.validity_restriction) {
        auto start_and_end = boost::get<StartAndEndValidity>(&validity_restriction);
        auto renewal_deadline = convert_time32(m_time_now + std::chrono::hours(1));
        if (start_and_end && start_and_end->end_validity < renewal_deadline) {
            m_own_certificate = generate_certificate(m_own_key_pair);
            break;
        }
    }

    return m_own_certificate;
}

const ecdsa256::PrivateKey& NaiveCertificateManager::own_private_key()
{
    return m_own_key_pair.private_key;
}

Certificate NaiveCertificateManager::generate_certificate(const ecdsa256::KeyPair& key_pair)
{
    // create certificate
    Certificate certificate;

    // section 6.1 in TS 103 097 v1.2.1
    certificate.signer_info = m_root_certificate_hash;

    // section 6.3 in TS 103 097 v1.2.1
    certificate.subject_info.subject_type = SubjectType::Authorization_Ticket;
    // section 7.4.2 in TS 103 097 v1.2.1, subject_name implicit empty

    // set assurance level
    certificate.subject_attributes.push_back(SubjectAssurance(0x00));

    // section 7.4.1 in TS 103 097 v1.2.1
    // set subject attributes
    // set the verification_key
    Uncompressed coordinates;
    coordinates.x.assign(key_pair.public_key.x.begin(), key_pair.public_key.x.end());
    coordinates.y.assign(key_pair.public_key.y.begin(), key_pair.public_key.y.end());
    EccPoint ecc_point = coordinates;
    ecdsa_nistp256_with_sha256 ecdsa;
    ecdsa.public_key = ecc_point;
    VerificationKey verification_key;
    verification_key.key = ecdsa;
    certificate.subject_attributes.push_back(verification_key);

    // section 6.7 in TS 103 097 v1.2.1
    // set validity restriction
    StartAndEndValidity start_and_end;
    start_and_end.start_validity = convert_time32(m_time_now - std::chrono::hours(1));
    start_and_end.end_validity = convert_time32(m_time_now + std::chrono::hours(23));
    certificate.validity_restriction.push_back(start_and_end);

    // set signature
    ByteBuffer data_buffer = convert_for_signing(certificate);
    certificate.signature = m_crypto_backend.sign_data(m_root_key_pair.private_key, data_buffer);

    return certificate;
}

CertificateValidity NaiveCertificateManager::check_certificate(const Certificate& certificate)
{
    // check validity restriction
    for (auto& restriction : certificate.validity_restriction) {
        ValidityRestriction validity_restriction = restriction;

        ValidityRestrictionType type = get_type(validity_restriction);
        if (type == ValidityRestrictionType::Time_Start_And_End) {
            // change start and end time of certificate validity
            StartAndEndValidity start_and_end = boost::get<StartAndEndValidity>(validity_restriction);
            // check if certificate validity restriction timestamps are logically correct
            if (start_and_end.start_validity >= start_and_end.end_validity) {
                return CertificateInvalidReason::BROKEN_TIME_PERIOD;
            }
            // check if certificate is premature or outdated
            auto now = convert_time32(m_time_now);
            if (now < start_and_end.start_validity || now > start_and_end.end_validity) {
                return CertificateInvalidReason::OFF_TIME_PERIOD;
            }
        }
    }

    // check if subject_name is empty
    if (0 != certificate.subject_info.subject_name.size()) {
        return CertificateInvalidReason::INVALID_NAME;
    }

    // check signer info
    if(get_type(certificate.signer_info) == SignerInfoType::Certificate_Digest_With_SHA256) {
        HashedId8 signer_hash = boost::get<HashedId8>(certificate.signer_info);
        if(signer_hash != m_root_certificate_hash) {
            return CertificateInvalidReason::INVALID_ROOT_HASH;
        }
    }

    // try to extract ECDSA signature
    boost::optional<EcdsaSignature> sig = extract_ecdsa_signature(certificate.signature);
    if (!sig) {
        return CertificateInvalidReason::MISSING_SIGNATURE;
    }

    // create buffer of certificate
    ByteBuffer cert = convert_for_signing(certificate);

    if (!m_crypto_backend.verify_data(m_root_key_pair.public_key, cert, sig.get())) {
        return CertificateInvalidReason::INVALID_SIGNATURE;
    }

    return CertificateValidity::valid();
}

const ecdsa256::KeyPair& NaiveCertificateManager::root_key_pair()
{
    static ecdsa256::KeyPair root = m_crypto_backend.generate_key_pair();
    return root;
}

} // namespace security
} // namespace vanetza
