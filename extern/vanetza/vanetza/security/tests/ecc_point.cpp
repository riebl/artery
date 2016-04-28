#include <gtest/gtest.h>
#include <vanetza/common/byte_sequence.hpp>
#include <vanetza/security/ecc_point.hpp>
#include <vanetza/security/public_key.hpp>
#include <vanetza/security/tests/check_ecc_point.hpp>

using vanetza::ByteBuffer;
using namespace vanetza::security;
using namespace vanetza;
using namespace std;

static const std::size_t length = field_size(PublicKeyAlgorithm::Ecdsa_Nistp256_With_Sha256);

EccPoint serialize_roundtrip(const EccPoint& point)
{
    EccPoint outPoint;
    std::stringstream stream;
    OutputArchive oa(stream);
    serialize(oa, point, PublicKeyAlgorithm::Ecdsa_Nistp256_With_Sha256);
    InputArchive ia(stream);
    deserialize(ia, outPoint, PublicKeyAlgorithm::Ecdsa_Nistp256_With_Sha256);
    return outPoint;
}

TEST(EccPoint, uncompressed)
{
    EccPoint point = Uncompressed { random_byte_sequence(length, 1), random_byte_sequence(length, 2) };
    EccPoint outPoint = serialize_roundtrip(point);
    check(point, outPoint);
    EXPECT_EQ(EccPointType::Uncompressed, get_type(outPoint));
}

TEST(EccPoint, Compressed_Lsb_Y_0)
{
    EccPoint point = Compressed_Lsb_Y_0 { random_byte_sequence(length, 3) };
    EccPoint outPoint = serialize_roundtrip(point);
    check(point, outPoint);
    EXPECT_EQ(EccPointType::Compressed_Lsb_Y_0, get_type(outPoint));
}

TEST(EccPoint, X_Coordinate_Only)
{
    EccPoint point = X_Coordinate_Only { random_byte_sequence(length, 4) };
    EccPoint outPoint = serialize_roundtrip(point);
    check(point, outPoint);
    EXPECT_EQ(EccPointType::X_Coordinate_Only, get_type(outPoint));
}

