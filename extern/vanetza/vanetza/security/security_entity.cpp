#include <vanetza/security/security_entity.hpp>
#include <vanetza/security/backend.hpp>
#include <vanetza/security/certificate_manager.hpp>
#include <vanetza/security/its_aid.hpp>
#include <future>
#include <mutex>
#include <stdexcept>
#include <string>

namespace vanetza
{
namespace security
{

SecurityEntity::SecurityEntity(const Clock::time_point& time_now, const std::string& backend) :
    m_time_now(time_now), m_sign_deferred(false),
    m_certificate_manager(time_now),
    m_crypto_backend(create_backend(backend))
{
    if (!m_crypto_backend) {
        std::string msg = "No crypto backend available with name \"" + backend + "\"";
        throw std::runtime_error(msg);
    }
}

SecurityEntity::~SecurityEntity()
{
    // only defined here so unique_ptr members can be used with incomplete types
}

EncapConfirm SecurityEntity::encapsulate_packet(const EncapRequest& encap_request)
{
    return sign(encap_request);
}

DecapConfirm SecurityEntity::decapsulate_packet(const DecapRequest& decap_request)
{
    return verify(decap_request);
}

EncapConfirm SecurityEntity::sign(const EncapRequest& request)
{
    EncapConfirm encap_confirm;
    // set secured message data
    encap_confirm.sec_packet.payload.type = PayloadType::Signed;
    encap_confirm.sec_packet.payload.data = std::move(request.plaintext_payload);
    // set header field data
    encap_confirm.sec_packet.header_fields.push_back(convert_time64(m_time_now));
    encap_confirm.sec_packet.header_fields.push_back(itsAidCa);

    SignerInfo signer_info = m_certificate_manager.own_certificate();
    encap_confirm.sec_packet.header_fields.push_back(signer_info);

    const size_t signature_size = get_size(signature_placeholder());
    const size_t trailer_size = get_size(TrailerField { signature_placeholder() });
    const auto& private_key = m_certificate_manager.own_private_key();

    if (m_sign_deferred) {
        auto future = std::async(std::launch::deferred, [=]() {
            ByteBuffer data = convert_for_signing(encap_confirm.sec_packet, trailer_size);
            return m_crypto_backend->sign_data(private_key, data);
        });
        EcdsaSignatureFuture signature(future.share(), signature_size);
        encap_confirm.sec_packet.trailer_fields.push_back(signature);
    } else {
        ByteBuffer data_buffer = convert_for_signing(encap_confirm.sec_packet, trailer_size);
        TrailerField trailer_field = m_crypto_backend->sign_data(private_key, data_buffer);
        assert(get_size(trailer_field) == trailer_size);
        encap_confirm.sec_packet.trailer_fields.push_back(trailer_field);
    }

    return encap_confirm;
}

DecapConfirm SecurityEntity::verify(const DecapRequest& request)
{
    DecapConfirm decap_confirm;

    const SecuredMessage& secured_message = request.sec_packet;
    // set the payload, when verfiy != success, we need this for NON_STRICT packet handling
    decap_confirm.plaintext_payload = request.sec_packet.payload.data;

    if (PayloadType::Signed != secured_message.payload.type) {
        decap_confirm.report = ReportType::Unsigned_Message;
        return decap_confirm;
    }

    if (SecuredMessage().protocol_version() != secured_message.protocol_version()) {
        decap_confirm.report = ReportType::Incompatible_Protocol;
        return decap_confirm;
    }

    boost::optional<const Certificate&> certificate;
    boost::optional<Time64> generation_time;
    for (auto& field : request.sec_packet.header_fields) {
        switch (get_type(field)) {
        case HeaderFieldType::Signer_Info:
            switch (get_type(boost::get<SignerInfo>(field))) {
            case SignerInfoType::Certificate:
                certificate = boost::get<Certificate>(boost::get<SignerInfo>(field));
                break;
            case SignerInfoType::Self:
            case SignerInfoType::Certificate_Digest_With_SHA256:
            case SignerInfoType::Certificate_Digest_With_Other_Algorithm:
                break;
            case SignerInfoType::Certificate_Chain:
                //TODO check if Certificate_Chain is inconsistant
                break;
            default:
                decap_confirm.report = ReportType::Unsupported_Signer_Identifier_Type;
                return decap_confirm;
                break;
            }
            break;
        case HeaderFieldType::Generation_Time:
            generation_time = boost::get<Time64>(field);
            break;
        default:
            break;
        }
    }

    if (!certificate) {
        decap_confirm.report = ReportType::Signer_Certificate_Not_Found;
        return decap_confirm;
    }

    if (!generation_time || !check_generation_time(*generation_time)) {
        decap_confirm.report = ReportType::Invalid_Timestamp;
        return decap_confirm;
    }

    // TODO check Duplicate_Message, Invalid_Mobility_Data, Unencrypted_Message, Decryption_Error

    boost::optional<ecdsa256::PublicKey> public_key = get_public_key(certificate.get());

    // public key could not be extracted
    if (!public_key) {
        decap_confirm.report = ReportType::Invalid_Certificate;
        return decap_confirm;
    }

    // if certificate could not be verified return correct ReportType
    CertificateValidity cert_validity = m_certificate_manager.check_certificate(*certificate);
    if (!cert_validity) {
        decap_confirm.report = ReportType::Invalid_Certificate;
        decap_confirm.certificate_validity = cert_validity;
        return decap_confirm;
    }

    // TODO check if Revoked_Certificate

    // convert signature byte buffer to string
    auto& trailer_fields = secured_message.trailer_fields;

    if (trailer_fields.empty()) {
        decap_confirm.report = ReportType::Unsigned_Message;
        return decap_confirm;
    }

    boost::optional<Signature> signature;
    for (auto& field : trailer_fields) {
        if (TrailerFieldType::Signature == get_type(field)) {
            if (PublicKeyAlgorithm::Ecdsa_Nistp256_With_Sha256 == get_type(boost::get<Signature>(field))) {
                signature = boost::get<Signature>(field);
                break;
            }
        }
    }

    // check Signature
    if (!signature) {
        decap_confirm.report = ReportType::Unsigned_Message;
        return decap_confirm;
    }

    // check the size of signature.R and siganture.s
    auto ecdsa = extract_ecdsa_signature(signature.get());
    const auto field_len = field_size(PublicKeyAlgorithm::Ecdsa_Nistp256_With_Sha256);
    if (!ecdsa || ecdsa->s.size() != field_len) {
        decap_confirm.report = ReportType::False_Signature;
        return decap_confirm;
    }

    // convert message byte buffer to string
    ByteBuffer payload = convert_for_signing(secured_message, get_size(TrailerField(signature.get())));

    // result of verify function
    bool result = m_crypto_backend->verify_data(public_key.get(), payload, ecdsa.get());

    if (result) {
        decap_confirm.report = ReportType::Success;
    } else {
        decap_confirm.report = ReportType::False_Signature;
    }

    return decap_confirm;
}

void SecurityEntity::enable_deferred_signing(bool flag)
{
    m_sign_deferred = flag;
}

const Signature& SecurityEntity::signature_placeholder() const
{
    static std::once_flag once_flag;
    static Signature signature;
    std::call_once(once_flag, [](Signature& signature) {
        const auto size = field_size(PublicKeyAlgorithm::Ecdsa_Nistp256_With_Sha256);
        EcdsaSignature ecdsa;
        ecdsa.s.resize(size, 0x00);
        X_Coordinate_Only coordinate;
        coordinate.x.resize(size, 0x00);
        ecdsa.R = coordinate;
        signature = ecdsa;
    }, std::ref(signature));
    return signature;
}

bool SecurityEntity::check_generation_time(Time64 generation_time) const
{
    // Values are picked from C2C-CC Basic System Profile v1.1.0, see RS_BSP_168
    const auto generation_time_future = std::chrono::milliseconds(40);
    const auto generation_time_past = std::chrono::minutes(10);
    // TODO generation_time_past for CAMs is only 2 seconds

    bool valid = true;
    if (generation_time > convert_time64(m_time_now + generation_time_future)) {
        valid = false;
    } else if (generation_time < convert_time64(m_time_now - generation_time_past)) {
        valid = false;
    }

    return valid;
}

} // namespace security
} // namespace vanetza
