#ifndef NAIVE_CERTIFICATE_MANAGER_HPP_MTULFLKX
#define NAIVE_CERTIFICATE_MANAGER_HPP_MTULFLKX

#include <vanetza/common/clock.hpp>
#include <vanetza/security/backend_cryptopp.hpp>
#include <vanetza/security/certificate_manager.hpp>

namespace vanetza
{
namespace security
{

/**
 * \brief A very simplistic certificate manager
 *
 * This certificate manager is INSECURE!
 * It's only okay for experimenting with flawed secured messages.
 */
class NaiveCertificateManager : public CertificateManager
{
public:
    NaiveCertificateManager(const Clock::time_point& time_now);

    /**
     * \brief check certificate
     * \param certificate to verify
     * \return certificate status
     */
    CertificateValidity check_certificate(const Certificate& certificate) override;

    /**
     * \brief get own certificate for signing
     * \return own certificate
     */
    const Certificate& own_certificate() override;

    /**
     * \brief get own private key
     * \return private key
     */
    const ecdsa256::PrivateKey& own_private_key() override;

private:
    /**
     * \brief generate a certificate
     *
     * \param key_pair keys used to create the certificate
     * \return generated certificate
     */
    Certificate generate_certificate(const ecdsa256::KeyPair& key_pair);

    /**
     * \brief retrieve common root key pair (for all instances)
     * \return root key pair
     */
    const ecdsa256::KeyPair& root_key_pair();

    BackendCryptoPP m_crypto_backend; /*< key generation is not a generic backend feature */
    const Clock::time_point& m_time_now;
    const ecdsa256::KeyPair& m_root_key_pair;
    HashedId8 m_root_certificate_hash;
    ecdsa256::KeyPair m_own_key_pair;
    Certificate m_own_certificate;
};

} // namespace securityurity
} // namespace vanetza

#endif /* NAIVE_CERTIFICATE_MANAGER_HPP_MTULFLKX */

