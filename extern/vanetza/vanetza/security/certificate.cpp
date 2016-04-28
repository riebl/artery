#include <vanetza/common/byte_buffer_sink.hpp>
#include <vanetza/security/certificate.hpp>
#include <vanetza/security/length_coding.hpp>
#include <vanetza/security/signer_info.hpp>
#include <vanetza/security/exception.hpp>
#include <boost/iostreams/stream.hpp>

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

} // ns security
} // ns vanetza
