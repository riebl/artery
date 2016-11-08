#ifndef SECURITY_ENTITY_HPP
#define SECURITY_ENTITY_HPP

#include <vanetza/common/clock.hpp>
#include <vanetza/security/certificate_manager.hpp>
#include <vanetza/security/decap_confirm.hpp>
#include <vanetza/security/decap_request.hpp>
#include <vanetza/security/encap_confirm.hpp>
#include <vanetza/security/encap_request.hpp>
#include <memory>
#include <string>

namespace vanetza
{
namespace security
{

// forward declaration
class Backend;

class SecurityEntity
{
public:
    /**
     * \param time_now timestamp referring to current time
     * \param backend [optional] identifier of desired backend implementation
     */
    SecurityEntity(const Clock::time_point& time_now, const std::string& backend = "default");
    ~SecurityEntity();

    /**
     * \brief Creates a security envelope covering the given payload.
     *
     * The payload consists of the CommonHeader, ExtendedHeader and the payload of
     * the layers above the network layer. The entire security envelope is used
     * to calculate a signature which gets added to the resulting SecuredMessage.
     *
     * \param request containing payload to sign
     * \return confirmation containing signed SecuredMessage
     */
    EncapConfirm encapsulate_packet(const EncapRequest& encap_request);

    /** \brief decapsulates packet
     *
     * \param packet that should be decapsulated
     * \return decapsulated packet
     */

    /**
     * \brief Decapsulates the payload within a SecuredMessage
     *
     * Verifies the Signature and SignerInfo of a SecuredMessage.
     *
     * \param request containing a SecuredMessage
     * \return decapsulation confirmation including plaintext payload
     */
    DecapConfirm decapsulate_packet(const DecapRequest& decap_request);

    /**
     * \brief enable deferred signature creation
     *
     * SecuredMessages contain EcdsaSignatureFuture instead of EcdsaSignature
     * when this feature is enabled.
     *
     * \param flag true for enabling deferred signature calculation
     */
    void enable_deferred_signing(bool flag);

private:
    /** \brief sign the packet
    *
    * \param encap request that was handed over
    * \return signed packet
    */
    EncapConfirm sign(const EncapRequest& encap_request);

    /** \brief verify the packet
    *
    * \param signed packet
    * \return verified packet
    */
    DecapConfirm verify(const DecapRequest& decap_request);

    /**
     * \brief signature used as placeholder until final signature is calculated
     * \return placeholder containing dummy data
     */
    const Signature& signature_placeholder() const;

    /**
     * \brief check if generation time is within validity range
     * \param gt generation time
     * \return true if valid
     */
    bool check_generation_time(Time64 gt) const;

    const Clock::time_point& m_time_now;
    bool m_sign_deferred; /*< controls if EcdsaSignatureFuture is used */
    NaiveCertificateManager m_certificate_manager; /*< replace with interface later */
    std::unique_ptr<Backend> m_crypto_backend;
};

} // namespace security
} // namespace vanetza

#endif // SECURITY_ENTITY_HPP
