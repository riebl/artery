#ifndef HEADER_FIELD_HPP_IHIAKD4K
#define HEADER_FIELD_HPP_IHIAKD4K

#include <vanetza/security/basic_elements.hpp>
#include <vanetza/security/certificate.hpp>
#include <vanetza/security/encryption_parameter.hpp>
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
    Message_Type = 5,                       // uint16 -> uint16be_t
    Signer_Info = 128,                      // SignerInfo
    Encryption_Parameters = 129,            // EncryptionParameters
    Recipient_Info = 130,                   // std::list<RecipientInfo>
};

/// HeaderField specified in TS 103 097 v1.2.1, section 5.4
using HeaderField =  boost::variant<
    Time64,
    Time64WithStandardDeviation,
    Time32,
    ThreeDLocation,
    std::list<HashedId3>,
    uint16_t,
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

} // namespace security
} // namespace vanetza

#endif /* HEADER_FIELD_HPP_IHIAKD4K */
