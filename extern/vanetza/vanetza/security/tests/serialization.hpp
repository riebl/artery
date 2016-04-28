#ifndef SERIALIZATION_HPP_ZWGI3RCG
#define SERIALIZATION_HPP_ZWGI3RCG

#include <vanetza/common/byte_buffer.hpp>
#include <vanetza/common/byte_buffer_source.hpp>
#include <vanetza/security/serialization.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <sstream>

namespace vanetza
{
namespace security
{

/**
 * \brief create a ByteBuffer from griven hex string
 * \param string hex string
 * \return equivalent byte buffer
 */
ByteBuffer buffer_from_hexstring(const char*);

/**
 * \brief Deserialize any class from given hex string
 * \tparam T the target type of deserialization
 * \tparam ARGS additional arguments passed to deserialize function
 * \param string hex string
 * \param result deserialize into this object
 * \param args additional arguments for deserialization
 */
template<typename T, typename... ARGS>
size_t deserialize_from_hexstring(const char* string, T& result, ARGS&&... args)
{
    auto buffer = buffer_from_hexstring(string);
    byte_buffer_source source(buffer);
    boost::iostreams::stream_buffer<byte_buffer_source> stream(source);
    InputArchive ar(stream, boost::archive::no_header);
    return deserialize(ar, result, std::forward<ARGS>(args)...);
}

/**
 * \brief Serialize and deserialize an object
 *
 * Source object is serialized and deserialized
 * object form this binary representation is returned.
 *
 * \tparam T the type of the result
 * \tparam ARGS additional arguments passed to underlying functions
 * \param source serialize from this object
 * \param args additional arguments
 * \return deserialized object (should be equal to source)
 */
template<typename T, typename... ARGS>
T serialize_roundtrip(const T& source, ARGS&&... args)
{
    std::stringstream stream;
    OutputArchive oa(stream);
    serialize(oa, source, std::forward<ARGS>(args)...);

    T result;
    InputArchive ia(stream);
    deserialize(ia, result, std::forward<ARGS>(args)...);

    return result;
}

} // namespace security
} // namespace vanetza

#endif /* SERIALIZATION_HPP_ZWGI3RCG */
