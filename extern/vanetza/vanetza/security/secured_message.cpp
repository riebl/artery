#include <vanetza/common/byte_buffer.hpp>
#include <vanetza/common/byte_buffer_sink.hpp>
#include <vanetza/security/exception.hpp>
#include <vanetza/security/secured_message.hpp>
#include <vanetza/security/serialization.hpp>
#include <boost/iostreams/stream.hpp>

namespace vanetza
{
namespace security
{

size_t get_size(const SecuredMessage& message)
{
    size_t size = sizeof(uint8_t); // protocol version
    size += get_size(message.header_fields);
    size += length_coding_size(get_size(message.header_fields));
    size += get_size(message.trailer_fields);
    size += length_coding_size(get_size(message.trailer_fields));
    size += get_size(message.payload);
    return size;
}

void serialize(OutputArchive& ar, const SecuredMessage& message)
{
    const uint8_t protocol_version = message.protocol_version();
    ar << protocol_version;
    serialize(ar, message.header_fields);
    serialize(ar, message.payload);
    serialize(ar, message.trailer_fields);
}

size_t deserialize(InputArchive& ar, SecuredMessage& message)
{
    uint8_t protocol_version = 0;
    ar >> protocol_version;
    size_t length = sizeof(protocol_version);
    if (protocol_version == 2) {
        const size_t hdr_length = deserialize(ar, message.header_fields);
        length += hdr_length + length_coding_size(hdr_length);
        length += deserialize(ar, message.payload);
        const size_t trlr_length = deserialize(ar, message.trailer_fields);
        length += trlr_length + length_coding_size(trlr_length);
    } else {
        throw deserialization_error("Unsupported SecuredMessage protocol version");
    }
    return length;
}

ByteBuffer convert_for_signing(const SecuredMessage& message, size_t trailer_fields_size)
{
    ByteBuffer buf;
    byte_buffer_sink sink(buf);

    boost::iostreams::stream_buffer<byte_buffer_sink> stream(sink);
    OutputArchive ar(stream, boost::archive::no_header);

    const uint8_t protocol_version = message.protocol_version();
    ar << protocol_version;
    serialize(ar, message.header_fields);
    serialize(ar, message.payload);

    ar << trailer_fields_size;

    return buf;
}

} // namespace security
} // namespace vanetza
