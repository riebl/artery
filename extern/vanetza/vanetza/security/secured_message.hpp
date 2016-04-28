#ifndef SECURED_MESSAGE_HPP_MO3HBSXG
#define SECURED_MESSAGE_HPP_MO3HBSXG

#include <vanetza/security/header_field.hpp>
#include <vanetza/security/trailer_field.hpp>
#include <vanetza/security/payload.hpp>
#include <cstdint>
#include <list>

namespace vanetza
{
namespace security
{

/// SecuredMessage as specified in TS 103 097 v1.2.1, section 5.1
struct SecuredMessageV2
{
    std::list<HeaderField> header_fields;
    std::list<TrailerField> trailer_fields;
    Payload payload;

    unsigned protocol_version() const { return 2; }
};

using SecuredMessage = SecuredMessageV2;

/**
 * \brief Calculates size of a SecuredMessage object
 * \return size_t containing the number of octets needed to serialize the object
 */
size_t get_size(const SecuredMessage&);

/**
 * \brief Serializes a SecuredMessage into a binary archive
 */
void serialize(OutputArchive& ar, const SecuredMessage& message);

/**
 * \brief Deserializes a SecuredMessage from a binary archive
 * \return size of deserialized SecuredMessage
 */
size_t deserialize(InputArchive& ar, SecuredMessage& message);

/**
 * \brief Create ByteBuffer equivalent of SecuredMessage suitable for signature creation
 *
 * ByteBuffer contains message's version, header_fields and payload.
 * Additionally, the length of trailer fields is appended.
 *
 * \param message
 * \param trailer_fields_size Length of trailer fields in bytes
 * \return serialized data fields relevant for signature creation
 */
ByteBuffer convert_for_signing(const SecuredMessage& message, size_t trailer_fields_size);

} // namespace security
} // namespace vanetza

#endif /* SECURED_MESSAGE_HPP_MO3HBSXG */
