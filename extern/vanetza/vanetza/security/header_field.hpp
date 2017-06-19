#ifndef HEADER_FIELD_HPP_IHIAKD4K
#define HEADER_FIELD_HPP_IHIAKD4K

#include <vanetza/security/basic_elements.hpp>
#include <vanetza/security/certificate.hpp>
#include <vanetza/security/encryption_parameter.hpp>
#include <vanetza/security/int_x.hpp>
#include <vanetza/security/recipient_info.hpp>
#include <vanetza/security/region.hpp>
#include <vanetza/security/signer_info.hpp>
#include <boost/variant/variant.hpp>
#include <list>

namespace vanetza
{
namespace security
{

/// HeaderFieldType specified in TS 103 097 v1.2.1, section 5.5
enum class HeaderFieldType : uint8_t
{
    Generation_Time = 0,                    // Time64
    Generation_Time_Confidence = 1,         // Time64WithStandardDeviation
    Expiration = 2,                         // Time32
    Generation_Location = 3,                // TreeDLocation
    Request_Unrecognized_Certificate = 4,   // std::list<HashedId3>
    Its_Aid = 5,                            // IntX
    Signer_Info = 128,                      // SignerInfo
    Encryption_Parameters = 129,            // EncryptionParameters
    Recipient_Info = 130,                   // std::list<RecipientInfo>
};

/// HeaderField specified in TS 103 097 v1.2.1, section 5.4
using HeaderField = boost::variant<
    Time64,
    Time64WithStandardDeviation,
    Time32,
    ThreeDLocation,
    std::list<HashedId3>,
    IntX,
    SignerInfo,
    EncryptionParameter,
    std::list<RecipientInfo>
>;

/**
 * \brief Determines HeaderFieldType to a given HeaderField
 * \param field
 * \return type
 */
HeaderFieldType get_type(const HeaderField& field);

/**
 * \brief Calculates size of a HeaderField
 * \param field
 * \return number of octets needed to serialize the HeaderField
 */
size_t get_size(const HeaderField& field);

/**
 * \brief Serializes a HeaderField into a binary archive
 * \note Serialization of HeaderField lists is provided by template
 * \param ar to serialize in
 * \param field to serialize
 */
void serialize(OutputArchive& ar, const HeaderField& field);

/**
 * \brief Deserializes a list of HeaderFields from a binary archive
 * \param ar with a serialized list of HeaderFields at the beginning
 * \param list of HeaderFields to deserialize
 * \return size of the deserialized list
 */
size_t deserialize(InputArchive& ar, std::list<HeaderField>& list);

/**
 * \brief resolve type for matching HeaderFieldType
 *
 * This is kind of the reverse function of get_type(const HeaderField&)
 */
template<HeaderFieldType>
struct header_field_type;

template<>
struct header_field_type<HeaderFieldType::Generation_Time>
{
    using type = Time64;
};

template<>
struct header_field_type<HeaderFieldType::Generation_Time_Confidence>
{
    using type = Time64WithStandardDeviation;
};

template<>
struct header_field_type<HeaderFieldType::Expiration>
{
    using type = Time32;
};

template<>
struct header_field_type<HeaderFieldType::Generation_Location>
{
    using type = ThreeDLocation;
};

template<>
struct header_field_type<HeaderFieldType::Request_Unrecognized_Certificate>
{
    using type = std::list<HashedId3>;
};

template<>
struct header_field_type<HeaderFieldType::Its_Aid>
{
    using type = IntX;
};

template<>
struct header_field_type<HeaderFieldType::Signer_Info>
{
    using type = SignerInfo;
};

template<>
struct header_field_type<HeaderFieldType::Encryption_Parameters>
{
    using type = EncryptionParameter;
};

template<>
struct header_field_type<HeaderFieldType::Recipient_Info>
{
    using type = std::list<RecipientInfo>;
};

} // namespace security
} // namespace vanetza

#endif /* HEADER_FIELD_HPP_IHIAKD4K */
