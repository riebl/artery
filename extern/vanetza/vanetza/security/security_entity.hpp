#ifndef SECURITY_ENTITY_HPP
#define SECURITY_ENTITY_HPP

#include <vanetza/security/decap_confirm.hpp>
#include <vanetza/security/decap_request.hpp>
#include <vanetza/security/encap_confirm.hpp>
#include <vanetza/security/encap_request.hpp>
#include <vanetza/security/sign_service.hpp>
#include <vanetza/security/verify_service.hpp>

namespace vanetza
{
namespace security
{

class SecurityEntity
{
public:
    /**
     * \brief Create security entity from primitive services.
     *
     * A std::invalid_argument exception is thrown at construction
     * if any given service is not callable.
     *
     * \param sign SN-SIGN service
     * \param verify SN-VERIFY service
     */
    SecurityEntity(SignService sign, VerifyService verify);

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
    EncapConfirm encapsulate_packet(EncapRequest&& encap_request);

    /**
     * \brief Decapsulates the payload within a SecuredMessage
     *
     * Verifies the Signature and SignerInfo of a SecuredMessage.
     *
     * \param request containing a SecuredMessage
     * \return decapsulation confirmation including plaintext payload
     */
    DecapConfirm decapsulate_packet(DecapRequest&& decap_request);

private:
    SignService m_sign_service;
    VerifyService m_verify_service;
};

} // namespace security
} // namespace vanetza

#endif // SECURITY_ENTITY_HPP
