#ifndef SERIALIZATION_BUFFER_HPP_8G2XAHRG
#define SERIALIZATION_BUFFER_HPP_8G2XAHRG

#include <vanetza/common/byte_buffer.hpp>
#include <vanetza/common/byte_buffer_sink.hpp>
#include <vanetza/common/byte_buffer_source.hpp>
#include <vanetza/geonet/serialization.hpp>
#include <boost/iostreams/stream_buffer.hpp>

namespace vanetza
{
namespace geonet
{

/**
 * This function is deprecated.
 * It will be removed as soon as geonet::serialize signatures are
 * compatible with common::serialize_into_buffer
 */
template<typename T>
void serialize_into_buffer(const T& t, ByteBuffer& buf)
{
    byte_buffer_sink sink(buf);
    boost::iostreams::stream_buffer<byte_buffer_sink> stream(sink);
    OutputArchive ar(stream, boost::archive::no_header);
    serialize(t, ar);
}

/**
 * This function is deprecated.
 * It will be removed as soon as geonet::deserialize signatures are
 * compatible with common::deserialize_into_buffer
 */
template<typename T>
void deserialize_from_buffer(T& t, const ByteBuffer& buf)
{
    byte_buffer_source source(buf);
    boost::iostreams::stream_buffer<byte_buffer_source> stream(source);
    InputArchive ar(stream, boost::archive::no_header);
    deserialize(t, ar);
}

} // namespace geonet
} // namespace vanetza

#endif /* SERIALIZATION_BUFFER_HPP_8G2XAHRG */

