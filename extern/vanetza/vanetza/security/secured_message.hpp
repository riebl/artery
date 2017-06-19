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

    /**
     * Fetch pointer to first matching header field
     * \param type HeaderField has to match given type
     * \return matching HeaderField or nullptr
     */
    HeaderField* header_field(HeaderFieldType);

    /**
     * Fetch read-only pointer to first machting header field
     * \param type requested header field type
     * \return matching header field or nullptr
     */
    const HeaderField* header_field(HeaderFieldType type) const;

    /**
     * Fetch pointer to first matching trailer field
     * \param type TrailerField has to match given type
     * \return matching TrailerField or nullptr
     */
    TrailerField* trailer_field(TrailerFieldType);

    /**
     * Fetch read-only pointer of first matching trailer field
     * \param type request trailer field type
     * \return matching trailer field or nullptr
     */
    const TrailerField* trailer_field(TrailerFieldType type) const;

    template<HeaderFieldType T>
    typename header_field_type<T>::type* header_field()
    {
        using field_type = typename header_field_type<T>::type;
        HeaderField* field = header_field(T);
        return boost::get<field_type>(field);
    }

    template<HeaderFieldType T>
    const typename header_field_type<T>::type* header_field() const
    {
        using field_type = typename header_field_type<T>::type;
        const HeaderField* field = header_field(T);
        return boost::get<field_type>(field);
    }

    template<TrailerFieldType T>
    typename trailer_field_type<T>::type* trailer_field()
    {
        using field_type = typename trailer_field_type<T>::type;
        TrailerField* field = trailer_field(T);
        return boost::get<field_type>(field);
    }

    template<TrailerFieldType T>
    const typename trailer_field_type<T>::type* trailer_field() const
    {
        using field_type = typename trailer_field_type<T>::type;
        const TrailerField* field = trailer_field(T);
        return boost::get<field_type>(field);
    }
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
