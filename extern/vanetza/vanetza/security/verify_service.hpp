#ifndef VERIFY_SERVICE_HPP_BR4ISDBH
#define VERIFY_SERVICE_HPP_BR4ISDBH

#include <vanetza/common/byte_buffer.hpp>
#include <vanetza/security/certificate.hpp>
#include <vanetza/security/its_aid.hpp>
#include <vanetza/security/secured_message.hpp>
#include <functional>

namespace vanetza
{

// forward declaration
class Runtime;

namespace security
{

// forward declarations
class Backend;
class CertificateManager;

enum class VerificationReport
{
    Success,
    False_Signature,
    Invalid_Certificate,
    Revoked_Certificate,
    Inconsistant_Chain,
    Invalid_Timestamp,
    Duplicate_Message,
    Invalid_Mobility_Data,
    Unsigned_Message,
    Signer_Certificate_Not_Found,
    Unsupported_Signer_Identifier_Type,
    Incompatible_Protocol
};

// mandatory parameters of SN-VERIFY.request (TS 102 723-8 V1.1.1)
struct VerifyRequest
{
    VerifyRequest(const SecuredMessage& msg) : secured_message(msg) {}
    const SecuredMessage& secured_message; /*< contains security header and payload */
};

// parameters of SN-VERIFY.confirm (TS 102 723-8 V1.1.1)
struct VerifyConfirm
{
    VerificationReport report; // mandatory
    IntX its_aid; // mandatory
    ByteBuffer permissions; // mandatory
    CertificateValidity certificate_validity; // non-standard extension
};

/**
 * Equivalent of SN-VERIFY service in TS 102 723-8 V1.1.1
 */
using VerifyService = std::function<VerifyConfirm(VerifyRequest&&)>;

/**
 * Get verify service with basic certificate and signature checks
 * \param rt runtime
 * \param certs certificate manager
 * \param backend crypto backend
 * \return callable verify service
 */
VerifyService straight_verify_service(Runtime&, CertificateManager&, Backend&);

/**
 * Get insecure dummy verify service without any checks
 * \param report confirm report result
 * \param validity certificate validity result
 * \return callable verify service
 */
VerifyService dummy_verify_service(VerificationReport report, CertificateValidity validity);

} // namespace security
} // namespace vanetza

#endif /* VERIFY_SERVICE_HPP_BR4ISDBH */
