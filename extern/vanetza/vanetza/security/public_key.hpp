#ifndef PUBLIC_KEY_HPP_DRZFSERF
#define PUBLIC_KEY_HPP_DRZFSERF

#include <vanetza/security/ecc_point.hpp>
#include <boost/variant/variant.hpp>

namespace vanetza
{
namespace security
{

/// SymmetricAlgorithm specified in TS 103 097 v1.2.1, section 4.2.3
enum class SymmetricAlgorithm : uint8_t
{
    Aes128_Ccm = 0
};

/// PublicKeyAlgorithm specified in TS 103 097 v1.2.1, section 4.2.2
enum class PublicKeyAlgorithm : uint8_t
{
    Ecdsa_Nistp256_With_Sha256 = 0,
    Ecies_Nistp256 = 1
};

/// ecdsa_nistp256_with_sha256 specified in TS 103 097 v1.2.1, section 4.2.4
struct ecdsa_nistp256_with_sha256
{
    EccPoint public_key;
};

/// ecies_nistp256 specified in TS 103 097 v1.2.1, section 4.2.4
struct ecies_nistp256
{
    SymmetricAlgorithm supported_symm_alg;
    EccPoint public_key;
};

/// Profile specified in TS 103 097 v1.2.1, section 4.2.4
using PublicKey = boost::variant<ecdsa_nistp256_with_sha256, ecies_nistp256>;

/**
 * \brief Determines PublicKeyAlgorithm to a given PublicKey
 * \param public_key
 * \return algorithm type
 */
PublicKeyAlgorithm get_type(const PublicKey&);

/**
 * \brief Calculates size of a PublicKey
 * \param public_key
 * \return number of octets needed to serialize the PublicKey
 */
size_t get_size(const PublicKey&);

/**
 * \brief Deserializes a PublicKey from a binary archive
 * \param ar with a serialized PublicKey at the beginning
 * \param public_key to save deserialized values in
 * \return size of the deserialized publicKey
 */
size_t deserialize(InputArchive&, PublicKey&);

/**
 * \brief Serializes a PublicKey into a binary archive
 * \param ar to serialize in
 * \param public_key to serialize
 */
void serialize(OutputArchive&, const PublicKey&);

/**
 * \brief Determines field size related to algorithm
 * \param public_key_algorithm
 * \return required buffer size for related fields
 * */
std::size_t field_size(PublicKeyAlgorithm);

/**
 * \brief Determines field size related to algorithm
 * \param symmetric_algorithm
 * \return required buffer size for related fields
 */
std::size_t field_size(SymmetricAlgorithm);

} // namespace security
} // namespace vanetza

#endif /* PUBLIC_KEY_HPP_DRZFSERF */
