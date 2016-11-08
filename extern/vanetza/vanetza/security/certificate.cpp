#include <vanetza/common/byte_buffer_sink.hpp>
#include <vanetza/common/serialization_buffer.hpp>
#include <vanetza/security/certificate.hpp>
#include <vanetza/security/length_coding.hpp>
#include <vanetza/security/signer_info.hpp>
#include <vanetza/security/exception.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/get.hpp>
#include <boost/variant/static_visitor.hpp>
#include <cryptopp/sha.h>
#include <algorithm>
#include <array>
#include <cstdint>

namespace vanetza
{
namespace security
{

size_t get_size(const Certificate& cert)
{
    size_t size = sizeof(cert.version());
    size += get_size(cert.signer_info);
    size += get_size(cert.subject_info);
    size += get_size(cert.subject_attributes);
    size += length_coding_size(get_size(cert.subject_attributes));
    size += get_size(cert.validity_restriction);
    size += length_coding_size(get_size(cert.validity_restriction));
    size += get_size(cert.signature);
    return size;
}


void serialize(OutputArchive& ar, const Certificate& cert)
{
    serialize(ar, host_cast(cert.version()));
    serialize(ar, cert.signer_info);
    serialize(ar, cert.subject_info);
    serialize(ar, cert.subject_attributes);
    serialize(ar, cert.validity_restriction);
    serialize(ar, cert.signature);
}

size_t deserialize(InputArchive& ar, Certificate& cert)
{
    uint8_t version = 0;
    deserialize(ar, version);
    size_t size = sizeof(cert.version());
    if (2 == version) {
        size += deserialize(ar, cert.signer_info);
        size += deserialize(ar, cert.subject_info);
        size += deserialize(ar, cert.subject_attributes);
        size += length_coding_size(get_size(cert.subject_attributes));
        size += deserialize(ar, cert.validity_restriction);
        size += length_coding_size(get_size(cert.validity_restriction));
        size += deserialize(ar, cert.signature);
    } else {
        throw deserialization_error("Unsupported Certificate version");
    }

    return size;
}

ByteBuffer convert_for_signing(const Certificate& cert)
{
    ByteBuffer buf;
    byte_buffer_sink sink(buf);

    boost::iostreams::stream_buffer<byte_buffer_sink> stream(sink);
    OutputArchive ar(stream, boost::archive::no_header);

    const uint8_t version = cert.version();
    ar << version;
    serialize(ar, cert.signer_info);
    serialize(ar, cert.subject_info);
    serialize(ar, cert.subject_attributes);
    serialize(ar, cert.validity_restriction);

    return buf;
}

boost::optional<Uncompressed> get_uncompressed_public_key(const Certificate& cert)
{
    boost::optional<Uncompressed> public_key_coordinates;
    for (auto& attribute : cert.subject_attributes) {
        if (get_type(attribute) == SubjectAttributeType::Verification_Key) {
            const VerificationKey& verification_key = boost::get<VerificationKey>(attribute);
            const EccPoint& ecc_point = boost::get<ecdsa_nistp256_with_sha256>(verification_key.key).public_key;
            public_key_coordinates = boost::get<Uncompressed>(ecc_point);
            break;
        }
    }

    return public_key_coordinates;
}

boost::optional<ecdsa256::PublicKey> get_public_key(const Certificate& cert)
{
    auto unc = get_uncompressed_public_key(cert);
    boost::optional<ecdsa256::PublicKey> result;
    ecdsa256::PublicKey pub;
    if (unc && unc->x.size() == pub.x.size() && unc->y.size() == pub.y.size()) {
        std::copy_n(unc->x.begin(), pub.x.size(), pub.x.data());
        std::copy_n(unc->y.begin(), pub.y.size(), pub.y.data());
        result = std::move(pub);
    }
    return result;
}

HashedId8 calculate_hash(const Certificate& cert)
{
    Certificate canonical_cert = cert;

    // canonical encoding according to TS 103 097 V1.2.1, section 4.2.12
    boost::optional<EcdsaSignature> signature = extract_ecdsa_signature(cert.signature);
    if (signature) {
        struct canonical_visitor : public boost::static_visitor<EccPoint>
        {
            EccPoint operator()(const X_Coordinate_Only& x_only) const
            {
                return x_only;
            }

            EccPoint operator()(const Compressed_Lsb_Y_0& y0) const
            {
                return X_Coordinate_Only { y0.x };
            }

            EccPoint operator()(const Compressed_Lsb_Y_1& y1) const
            {
                return X_Coordinate_Only { y1.x };
            }

            EccPoint operator()(const Uncompressed& unc) const
            {
                return X_Coordinate_Only { unc.x };
            }
        };

        EcdsaSignature canonical_sig;
        canonical_sig.s = signature->s;
        canonical_sig.R = boost::apply_visitor(canonical_visitor(), signature->R);
        assert(get_type(canonical_sig.R) == EccPointType::X_Coordinate_Only);
        canonical_cert.signature = canonical_sig;
    }

    ByteBuffer bytes;
    serialize_into_buffer(canonical_cert, bytes);

    CryptoPP::SHA256 hash;
    std::array<uint8_t, decltype(hash)::DIGESTSIZE> digest;
    hash.CalculateDigest(digest.data(), bytes.data(), bytes.size());

    HashedId8 id;
    assert(digest.size() >= id.size());
    std::copy(digest.end() - id.size(), digest.end(), id.begin());
    return id;
}

} // ns security
} // ns vanetza
