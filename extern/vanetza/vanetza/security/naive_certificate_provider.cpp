#include <vanetza/security/basic_elements.hpp>
#include <vanetza/security/ecc_point.hpp>
#include <vanetza/security/naive_certificate_provider.hpp>
#include <vanetza/security/payload.hpp>
#include <vanetza/security/secured_message.hpp>
#include <vanetza/security/signature.hpp>
#include <chrono>

namespace vanetza
{
namespace security
{

NaiveCertificateProvider::NaiveCertificateProvider(const Clock::time_point& time_now) :
    m_time_now(time_now),
    m_own_key_pair(m_crypto_backend.generate_key_pair()),
    m_own_certificate(generate_authorization_ticket()) { }

const Certificate& NaiveCertificateProvider::own_certificate()
{
    // renew certificate if necessary
    for (auto& validity_restriction : m_own_certificate.validity_restriction) {
        auto start_and_end = boost::get<StartAndEndValidity>(&validity_restriction);
        auto renewal_deadline = convert_time32(m_time_now + std::chrono::hours(1));
        if (start_and_end && start_and_end->end_validity < renewal_deadline) {
            m_own_certificate = generate_authorization_ticket();
            break;
        }
    }

    return m_own_certificate;
}

const ecdsa256::PrivateKey& NaiveCertificateProvider::own_private_key()
{
    return m_own_key_pair.private_key;
}

const ecdsa256::KeyPair& NaiveCertificateProvider::root_key_pair()
{
    static const ecdsa256::KeyPair root_key_pair = m_crypto_backend.generate_key_pair();

    return root_key_pair;
}

const Certificate& NaiveCertificateProvider::root_certificate()
{
    static const std::string root_subject("Naive Root CA");
    static const Certificate root_certificate = generate_root_certificate(root_subject);

    return root_certificate;
}

Certificate NaiveCertificateProvider::generate_authorization_ticket()
{
    // create certificate
    Certificate certificate;

    // section 6.1 in TS 103 097 v1.2.1
    certificate.signer_info = calculate_hash(root_certificate());

    // section 6.3 in TS 103 097 v1.2.1
    certificate.subject_info.subject_type = SubjectType::Authorization_Ticket;
    // section 7.4.2 in TS 103 097 v1.2.1, subject_name implicit empty

    // set assurance level
    certificate.subject_attributes.push_back(SubjectAssurance(0x00));

    // section 7.4.1 in TS 103 097 v1.2.1
    // set subject attributes
    // set the verification_key
    Uncompressed coordinates;
    coordinates.x.assign(m_own_key_pair.public_key.x.begin(), m_own_key_pair.public_key.x.end());
    coordinates.y.assign(m_own_key_pair.public_key.y.begin(), m_own_key_pair.public_key.y.end());
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
    certificate.signature = m_crypto_backend.sign_data(root_key_pair().private_key, data_buffer);

    return certificate;
}

Certificate NaiveCertificateProvider::generate_root_certificate(const std::string& root_subject)
{
    // create certificate
    Certificate certificate;

    // section 6.1 in TS 103 097 v1.2.1
    certificate.signer_info = nullptr; /* self */

    // section 6.3 in TS 103 097 v1.2.1
    certificate.subject_info.subject_type = SubjectType::Root_Ca;

    // section 7.4.2 in TS 103 097 v1.2.1
    std::vector<unsigned char> subject(root_subject.begin(), root_subject.end());
    certificate.subject_info.subject_name = subject;

    // section 6.6 in TS 103 097 v1.2.1 - levels currently undefined
    certificate.subject_attributes.push_back(SubjectAssurance(0x00));

    // section 7.4.1 in TS 103 097 v1.2.1
    // set subject attributes
    // set the verification_key
    Uncompressed coordinates;
    coordinates.x.assign(root_key_pair().public_key.x.begin(), root_key_pair().public_key.x.end());
    coordinates.y.assign(root_key_pair().public_key.y.begin(), root_key_pair().public_key.y.end());
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
    start_and_end.end_validity = convert_time32(m_time_now + std::chrono::hours(365 * 24));
    certificate.validity_restriction.push_back(start_and_end);

    // set signature
    ByteBuffer data_buffer = convert_for_signing(certificate);
    certificate.signature = m_crypto_backend.sign_data(root_key_pair().private_key, data_buffer);

    return certificate;
}

} // namespace security
} // namespace vanetza
