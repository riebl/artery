#ifndef ECC_POINT_HPP_XCESTUEB
#define ECC_POINT_HPP_XCESTUEB

#include <vanetza/common/byte_buffer.hpp>
#include <vanetza/security/serialization.hpp>
#include <boost/variant/variant.hpp>
#include <cstdint>

namespace vanetza
{
namespace security
{

/// forward declaration, see public_key.hpp
enum class PublicKeyAlgorithm: uint8_t;

/// EccPointType specified in TS 103 097 v1.2.1 in section 4.2.6
enum class EccPointType : uint8_t
{
    X_Coordinate_Only = 0,
    Compressed_Lsb_Y_0 = 2,
    Compressed_Lsb_Y_1 = 3,
    Uncompressed = 4
};

/// X_Coordinate_Only specified in TS 103 097 v1.2.1 in section 4.2.5
struct X_Coordinate_Only
{
    ByteBuffer x;
};

/// Compressed_Lsb_Y_0 specified in TS 103 097 v1.2.1 in section 4.2.5
struct Compressed_Lsb_Y_0
{
    ByteBuffer x;
};

/// Compressed_Lsb_Y_1 specified in TS 103 097 v1.2.1 in section 4.2.5
struct Compressed_Lsb_Y_1
{
    ByteBuffer x;
};

/// Uncompressed specified in TS 103 097 v1.2.1 in section 4.2.5
struct Uncompressed
{
    ByteBuffer x;
    ByteBuffer y;
};

/// EccPoint specified in TS 103 097 v1.2.1 in section 4.2.5
using EccPoint = boost::variant<
    X_Coordinate_Only,
    Compressed_Lsb_Y_0,
    Compressed_Lsb_Y_1,
    Uncompressed
>;

/**
 * \brief Determines EccPointType to a given EccPoint
 * \param ecc_point
 * \return type
 */
EccPointType get_type(const EccPoint&);

/**
 * \brief Serializes an EccPoint into a binary archive
 * \param ar to serialize in
 * \param ecc_point to serialize
 * \param pka Public key algorithm used for EccPoint
 */
void serialize(OutputArchive&, const EccPoint&, PublicKeyAlgorithm);

/**
 * \brief Deserializes an EccPoint from a binary archive
 * \param ar with a serialized EccPoint at the beginning,
 * \param ecc_point to deserialize
 * \param pka to get field size of the encoded coordinates
 */
void deserialize(InputArchive&, EccPoint&, PublicKeyAlgorithm);

/**
 * \brief Calculates size of an EccPoint
 * \param ecc_point
 * \return size_t containing the number of octets needed to serialize the EccPoint
 */
size_t get_size(const EccPoint&);

/**
 * \brief Convert EccPoint for signature calculation
 * Uses ecc_point.x as relevant field for signatures.
 *
 * \param ecc_point
 * \return binary representation of ECC point
 */
ByteBuffer convert_for_signing(const EccPoint& ecc_point);

} //namespace security
} //namespace vanetza

#endif /* ECC_POINT_HPP_XCESTUEB */
