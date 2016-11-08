#ifndef ECDSA256_HPP_IOXLJFVZ
#define ECDSA256_HPP_IOXLJFVZ

#include <array>
#include <cstdint>
#include <utility>

namespace vanetza
{
namespace security
{

// forward declaration
struct Uncompressed;

namespace ecdsa256
{

constexpr unsigned digest_octets = 32;

struct PublicKey
{
    std::array<uint8_t, digest_octets> x;
    std::array<uint8_t, digest_octets> y;
};

bool operator==(const PublicKey& lhs, const PublicKey& rhs);
bool operator!=(const PublicKey& lhs, const PublicKey& rhs);


struct PrivateKey
{
    std::array<uint8_t, digest_octets> key;
};

bool operator==(const PrivateKey& lhs, const PrivateKey& rhs);
bool operator!=(const PrivateKey& lhs, const PrivateKey& rhs);


struct KeyPair
{
    PrivateKey private_key;
    PublicKey public_key;
};

/**
 * Create generic public key from uncompressed ECC point
 * \param unc Uncompressed ECC point
 * \return public key
 */
PublicKey create_public_key(const Uncompressed&);

} // namespace ecdsa256
} // namespace security
} // namespace vanetza


namespace std
{

template<>
struct hash<vanetza::security::ecdsa256::PublicKey>
{
    size_t operator()(const vanetza::security::ecdsa256::PublicKey&) const;
};

template<>
struct hash<vanetza::security::ecdsa256::PrivateKey>
{
    size_t operator()(const vanetza::security::ecdsa256::PrivateKey&) const;
};

} // namespace std

#endif /* ECDSA256_HPP_IOXLJFVZ */

