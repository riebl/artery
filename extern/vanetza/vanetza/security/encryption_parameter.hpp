#ifndef ENCRYPTION_PARAMETER_HPP_EIAWNAWY
#define ENCRYPTION_PARAMETER_HPP_EIAWNAWY

#include <vanetza/security/serialization.hpp>
#include <boost/variant/variant.hpp>
#include <cstdint>

namespace vanetza
{
namespace security
{

/// forward declaration, see public_key.hpp
enum class SymmetricAlgorithm : uint8_t;

/// Nonce specified in TS 103 097 v1.2.1, section 4.2.7
using Nonce = std::array<uint8_t, 12>;

/// EncryptionParameter specified in TS 103 097 v1.2.1, section 4.2.7
using EncryptionParameter = boost::variant<Nonce>;

/**
 * \brief Determines SymmetricAlgorithm for an EncryptionParameter
 * \param param
 * \return SymmetricAlgorithm
 */
SymmetricAlgorithm get_type(const EncryptionParameter&);

/**
 * \brief Serializes an EncryptionParameter into a binary archive
 * \param ar to serialize in
 * \param param to serialize
 */
void serialize(OutputArchive&, const EncryptionParameter&);

/**
 * \brief Calculates size of an EncryptionParameter
 * \param param
 * \return number of octets needed to serialize the EncryptionParameter
 */
size_t get_size(const EncryptionParameter&);

/**
 * \brief Deserializes an EncryptionParameter from a binary archive
 * \param ar Input expected to start with an EncryptionParameter
 * \param enc Deserialized encryption parameter
 * \return size of deserialized EncryptionParameter
 */
size_t deserialize(InputArchive&, EncryptionParameter&);

} // namespace security
} // namespace vanetzta

#endif /* ENCRYPTION_PARAMETER_HPP_EIAWNAWY */
