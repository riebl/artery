#ifndef CERTIFICATE_MANAGER_HPP
#define CERTIFICATE_MANAGER_HPP

#include <vanetza/common/byte_buffer.hpp>
#include <vanetza/common/clock.hpp>
#include <vanetza/common/hook.hpp>
#include <vanetza/security/basic_elements.hpp>
#include <vanetza/security/decap_request.hpp>
#include <vanetza/security/encap_request.hpp>
#include <vanetza/security/decap_confirm.hpp>
#include <vanetza/security/encap_confirm.hpp>
#include <vanetza/security/trailer_field.hpp>
#include <vanetza/security/certificate.hpp>
#include <cryptopp/eccrypto.h>
#include <cryptopp/sha.h>
#include <string>

namespace vanetza
{
namespace security
{

/**
 * \brief A Manager to handle Certificates using Crypto++
 * \todo rename to CryptoPPCertManager
 * \todo create a base class
 */
class CertificateManager
{
public:
    typedef CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PrivateKey PrivateKey;
    typedef CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PublicKey PublicKey;
    typedef CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::Signer Signer;
    typedef CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::Verifier Verifier;

    enum class CertificateInvalidReason
    {
        BROKEN_TIME_PERIOD,
        OFF_TIME_PERIOD,
        INVALID_ROOT_HASH,
        MISSING_SIGNATURE,
        INVALID_SIGNATURE,
        INVALID_NAME,
    };

    struct KeyPair
    {
        PrivateKey private_key;
        PublicKey public_key;
    };

    Hook<CertificateInvalidReason> certificate_invalid;

    CertificateManager(const Clock::time_point& time_now);

    /**
     * \brief Creates an security envelope covering the given payload.
     *
     * The payload consists of the CommonHeader, ExtendedHeader and the payload of
     * the layers above the network layer. The entire security envelope is used
     * to calculate a signature which gets added to the resulting SecuredMessage.
     *
     * \param request containing payload to sign
     * \return confirmation containing signed SecuredMessage
     */
    EncapConfirm sign_message(const EncapRequest& request);

    /**
     * \brief Verifies the Signature and SignerInfo of a SecuredMessage
     *
     * It also decapsulates the data from the SecuredMessage.
     *
     * \param request containing a SecuredMessage
     * \return decapsulation confirmation
     */
    DecapConfirm verify_message(const DecapRequest& request);

    /**
     * \brief generate a certificate
     *
     * \param key_pair keys used to create the certificate
     * \return generated certificate
     */
    Certificate generate_certificate(const KeyPair& key_pair);

    /**
     * \brief enable deferred signature creation
     *
     * SecuredMessages contain EcdsaSignatureFuture instead of EcdsaSignature
     * when this feature is enabled.
     *
     * \param flag true for enabling deferred signature calculation
     */
    void enable_deferred_signing(bool flag);

    /**
     * \brief generate a private key and the corresponding public key
     * \return generated key pair
     */
     KeyPair generate_key_pair();

private:
    /**
     * \brief check the certificate
     *
     * \param certificate to verify
     * \return true if certificate could be verified
     */
    bool check_certificate(const Certificate& certificate);

    /**
     * \brief extract public key from a certificate
     *
     * \param certificate
     * \return PublicKey
     */
    boost::optional<PublicKey> get_public_key_from_certificate(const Certificate& certificate);

    /**
     * \brief get the current (system) time in microseconds
     * \return Time64
     */
    Time64 get_time();

    /**
     * \brief get the current (system) time in seconds
     * \return Time32
     */
    Time32 get_time_in_seconds();

    /**
     * \brief generate EcdsaSignature, for given data with private_key
     *
     * \param private_key used to sign the data
     * \param data_buffer the data
     * \return EcdsaSignature resulting signature
     */
    EcdsaSignature sign_data(const PrivateKey& private_key, const ByteBuffer& data_buffer);

    /**
     * \brief checks if the data_buffer can be verified with the public_key
     *
     * \param public_key
     * \param data to be verified
     * \param sig signature for verification
     * \return true if the data could be verified
     *
     */
    bool verify_data(const PublicKey& public_key, const ByteBuffer& data, const ByteBuffer& sig);

    /** \brief retrieve common root key pair (for all instances)
     *
     * \note This is only a temporary workaround!
     * \return root key pair
     */
    const KeyPair& get_root_key_pair();

    const Clock::time_point& m_time_now;
    const KeyPair& m_root_key_pair;
    HashedId8 m_root_certificate_hash;
    KeyPair m_own_key_pair;
    Certificate m_own_certificate;
    bool m_sign_deferred;
};

} // namespace security
} // namespace vanetza

#endif // CERTIFICATE_MANAGER_HPP
