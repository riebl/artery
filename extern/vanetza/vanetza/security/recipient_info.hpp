#ifndef RECIPIENT_INFO_HPP_IENLXEUN
#define RECIPIENT_INFO_HPP_IENLXEUN

#include <vanetza/security/basic_elements.hpp>
#include <vanetza/security/public_key.hpp>
#include <boost/variant/variant.hpp>

namespace vanetza
{
namespace security
{

/// EciesEncryptedKey specified in TS 103 097 v1.2.1, section 5.9
struct EciesEncryptedKey
{
    EccPoint v;
    ByteBuffer c;
    std::array<uint8_t, 16> t;
};

/// OpaqueKey specified in TS 103 097 v1.2.1, section 5.8
struct OpaqueKey
{
    ByteBuffer data;
};

/// Key specified in TS 103 097 v1.2.1, section 5.8 (in RecipientInfo)
typedef boost::variant<EciesEncryptedKey, OpaqueKey> Key;

/// RecipientInfo specified in TS 103 097 v1.2.1, section 5.8
struct RecipientInfo
{
    HashedId8 cert_id;
    Key enc_key;

    PublicKeyAlgorithm pk_encryption() const;
};

/**
 * \brief Determines applicable PublicKeyAlgorithm
 * \param key Algorithm has to fit this kind of key
 * \return PublicKeyAlgorithm
 */
PublicKeyAlgorithm get_type(const Key&);

/**
 * Calculates size of a RecipientInfo
 * \param info
 * \return number of octets needed to serialize the RecipientInfo
 */
size_t get_size(const RecipientInfo&);

/**
 * \brief Serializes a RecipientInfo into a binary archive
 * \param ar Destination of serialized object
 * \param info RecipientInfo to serialize
 * \param sym Applicable symmetric algorithm
 */
void serialize(OutputArchive&, const RecipientInfo&, SymmetricAlgorithm);

/**
 * \brief Deserialize a RecipientInfo
 * \param ar Input starting with serialized RecipientInfo
 * \param info Deserialized RecipientInfo
 * \param sym Symmetric algorithm required to deserialize encrypted key
 * \return size of the deserialized RecipientInfo in bytes
 */
size_t deserialize(InputArchive&, RecipientInfo&, const SymmetricAlgorithm&);

} // namespace security
} // namespace vanetza

#endif /* RECIPIENT_INFO_HPP_IENLXEUN */
