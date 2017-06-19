#include <vanetza/common/runtime.hpp>
#include <vanetza/security/backend.hpp>
#include <vanetza/security/certificate_manager.hpp>
#include <vanetza/security/its_aid.hpp>
#include <vanetza/security/verify_service.hpp>
#include <boost/optional.hpp>
#include <chrono>

namespace vanetza
{
namespace security
{
namespace
{

bool check_generation_time(Clock::time_point now, const SecuredMessageV2& message)
{
    using namespace std::chrono;

    bool valid = false;
    const HeaderField* generation_time_field = message.header_field(HeaderFieldType::Generation_Time);
    const Time64* generation_time = boost::get<Time64>(generation_time_field);
    if (generation_time) {
        // Values are picked from C2C-CC Basic System Profile v1.1.0, see RS_BSP_168
        static const auto generation_time_future = milliseconds(40);
        static const auto generation_time_past_default = minutes(10);;
        static const auto generation_time_past_ca = minutes(2);
        auto generation_time_past = generation_time_past_default;

        const HeaderField* its_aid_field = message.header_field(HeaderFieldType::Its_Aid);
        const IntX* its_aid = boost::get<IntX>(its_aid_field);
        if (its_aid && itsAidCa == *its_aid) {
            generation_time_past = generation_time_past_ca;
        }

        if (*generation_time > convert_time64(now + generation_time_future)) {
            valid = false;
        } else if (*generation_time < convert_time64(now - generation_time_past)) {
            valid = false;
        } else {
            valid = true;
        }
    }

    return valid;
}

} // namespace

VerifyService straight_verify_service(Runtime& rt, CertificateManager& certs, Backend& backend)
{
    return [&](VerifyRequest&& request) -> VerifyConfirm {
        VerifyConfirm confirm;
        const SecuredMessage& secured_message = request.secured_message;

        if (PayloadType::Signed != secured_message.payload.type) {
            confirm.report = VerificationReport::Unsigned_Message;
            return confirm;
        }

        if (2 != secured_message.protocol_version()) {
            confirm.report = VerificationReport::Incompatible_Protocol;
            return confirm;
        }

        const IntX* its_aid = secured_message.header_field<HeaderFieldType::Its_Aid>();
        confirm.its_aid = its_aid ? *its_aid : IntX(0);

        const SignerInfo* signer_info = secured_message.header_field<HeaderFieldType::Signer_Info>();
        boost::optional<const Certificate&> certificate;
        if (signer_info) {
            switch (get_type(*signer_info)) {
                case SignerInfoType::Certificate:
                    certificate = boost::get<Certificate>(*signer_info);
                    break;
                case SignerInfoType::Self:
                case SignerInfoType::Certificate_Digest_With_SHA256:
                case SignerInfoType::Certificate_Digest_With_Other_Algorithm:
                    break;
                case SignerInfoType::Certificate_Chain:
                    // TODO check if Certificate_Chain is inconsistant
                    break;
                default:
                    confirm.report = VerificationReport::Unsupported_Signer_Identifier_Type;
                    return confirm;
                    break;
            }
        }

        if (!certificate) {
            confirm.report = VerificationReport::Signer_Certificate_Not_Found;
            return confirm;
        }

        if (!check_generation_time(rt.now(), secured_message)) {
            confirm.report = VerificationReport::Invalid_Timestamp;
            return confirm;
        }

        // TODO check Duplicate_Message, Invalid_Mobility_Data, Unencrypted_Message, Decryption_Error

        boost::optional<ecdsa256::PublicKey> public_key = get_public_key(certificate.get());

        // public key could not be extracted
        if (!public_key) {
            confirm.report = VerificationReport::Invalid_Certificate;
            return confirm;
        }

        // if certificate could not be verified return correct DecapReport
        CertificateValidity cert_validity = certs.check_certificate(*certificate);
        if (!cert_validity) {
            confirm.report = VerificationReport::Invalid_Certificate;
            confirm.certificate_validity = cert_validity;
            return confirm;
        }

        // TODO check if Revoked_Certificate

        // check Signature
        const TrailerField* signature_field = secured_message.trailer_field(TrailerFieldType::Signature);
        const Signature* signature = boost::get<Signature>(signature_field);
        if (!signature) {
            confirm.report = VerificationReport::Unsigned_Message;
        } else if (PublicKeyAlgorithm::Ecdsa_Nistp256_With_Sha256 != get_type(*signature)) {
            confirm.report = VerificationReport::False_Signature;
        } else {
            // check the size of signature.R and siganture.s
            auto ecdsa = extract_ecdsa_signature(*signature);
            const auto field_len = field_size(PublicKeyAlgorithm::Ecdsa_Nistp256_With_Sha256);
            if (!ecdsa || ecdsa->s.size() != field_len) {
                confirm.report = VerificationReport::False_Signature;
            } else {
                // verify payload signature with given signature
                ByteBuffer payload = convert_for_signing(secured_message, get_size(*signature_field));
                if (backend.verify_data(public_key.get(), payload, *ecdsa)) {
                    confirm.report = VerificationReport::Success;
                } else {
                    confirm.report = VerificationReport::False_Signature;
                }
            }
        }

        return confirm;
    };
}

VerifyService dummy_verify_service(VerificationReport report, CertificateValidity validity)
{
    return [=](VerifyRequest&& request) -> VerifyConfirm {
        VerifyConfirm confirm;
        confirm.report = report;
        confirm.certificate_validity = validity;
        const IntX* its_aid = request.secured_message.header_field<HeaderFieldType::Its_Aid>();
        confirm.its_aid = its_aid ? *its_aid : IntX(0);
        return confirm;
    };
}

} // namespace security
} // namespace vanetza
