#include <gtest/gtest.h>
#include <vanetza/common/byte_sequence.hpp>
#include <vanetza/security/tests/check_ecc_point.hpp>
#include <vanetza/security/tests/check_signature.hpp>
#include <vanetza/security/tests/check_visitor.hpp>

namespace vanetza
{
namespace security
{

void check(const EcdsaSignature& expected, const EcdsaSignature& actual)
{
    check(expected.R, actual.R);
    EXPECT_EQ(expected.s, actual.s);
}

void check(const EcdsaSignatureFuture& expected, const EcdsaSignatureFuture& actual)
{
    check(expected.get(), actual.get());
}

void check(const Signature& expected, const Signature& actual)
{
    ASSERT_EQ(get_type(expected), get_type(actual));
    boost::apply_visitor(check_visitor<Signature>(), expected, actual);
}

EcdsaSignature create_random_ecdsa_signature(int seed)
{
    const std::size_t field = field_size(PublicKeyAlgorithm::Ecdsa_Nistp256_With_Sha256);
    EcdsaSignature signature;
    signature.s = random_byte_sequence(field, seed);
    signature.R = X_Coordinate_Only { random_byte_sequence(field, ~seed) };
    return signature;
}

} // namespace security
} // namespace vanetza
