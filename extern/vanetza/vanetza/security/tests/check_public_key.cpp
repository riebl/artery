#include <gtest/gtest.h>
#include <vanetza/common/byte_sequence.hpp>
#include <vanetza/security/tests/check_ecc_point.hpp>
#include <vanetza/security/tests/check_public_key.hpp>
#include <vanetza/security/tests/check_visitor.hpp>

namespace vanetza
{
namespace security
{

void check(const ecdsa_nistp256_with_sha256& expected, const ecdsa_nistp256_with_sha256& actual)
{
    check(expected.public_key, actual.public_key);
}

void check(const ecies_nistp256& expected, const ecies_nistp256& actual)
{
    EXPECT_EQ(expected.supported_symm_alg, actual.supported_symm_alg);
    check(expected.public_key, actual.public_key);
}

void check(const PublicKey& expected, const PublicKey& actual)
{
    ASSERT_EQ(get_type(expected), get_type(actual));
    boost::apply_visitor(check_visitor<PublicKey>(), expected, actual);
}

PublicKey create_random_public_key(int seed)
{
    const std::size_t size = field_size(PublicKeyAlgorithm::Ecies_Nistp256);
    EccPoint point = Uncompressed { random_byte_sequence(size, seed),
        random_byte_sequence(size, seed + 1) };
    ecies_nistp256 ecies;
    ecies.public_key = point;
    ecies.supported_symm_alg = SymmetricAlgorithm::Aes128_Ccm;

    return ecies;
}

} // namespace security
} // namespace vanetza
