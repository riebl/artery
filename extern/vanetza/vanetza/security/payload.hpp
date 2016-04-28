#ifndef PAYLOAD_HPP_R8IXQBSL
#define PAYLOAD_HPP_R8IXQBSL

#include <vanetza/common/byte_buffer.hpp>
#include <vanetza/net/packet.hpp>
#include <vanetza/security/serialization.hpp>
#include <cstdint>

namespace vanetza
{
namespace security
{

/// PayloadType specified in TS 103 097 v1.2.1, section 5.3
enum class PayloadType : uint8_t
{
    Unsecured = 0,
    Signed = 1,
    Encrypted = 2,
    Signed_External = 3,
    Signed_And_Encrypted = 4,
};

/// Payload specified in TS 103 097 v1.2.1, section 5.2
struct Payload
{
    PayloadType type;
    PacketVariant data;
};

/**
 * \brief Determines PayloadType to a given Payload
 * \param payload
 * \return type
 */
PayloadType get_type(const Payload&);

/**
 * \brief Calculates size of Payload
 * \param payload
 * \return number of octets needed to serialize the Payload
 */
size_t get_size(const Payload&);

/**
 * \brief Serializes Payload into a binary archive
 * \param ar to serialize in
 * \param payload to serialize
 */
void serialize(OutputArchive& ar, const Payload&);

/**
 * \brief Deserializes Payload from a binary archive
 * \param ar with serialized Payload at the beginning
 * \param payload to deserialize
 * \return size of the deserialized payload
 */
size_t deserialize(InputArchive& ar, Payload&);

} // namespace security
} // namespace vanetza

#endif /* PAYLOAD_HPP_R8IXQBSL */
