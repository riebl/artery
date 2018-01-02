#ifndef SHA_HPP_ENSVKDXU
#define SHA_HPP_ENSVKDXU

#include <array>
#include <cstdint>

namespace vanetza
{
namespace security
{

using Sha256Digest = std::array<uint8_t, 32>;

Sha256Digest calculate_sha256_digest(uint8_t* data, std::size_t len);

} // namespace security
} // namespace vanetza

#endif /* SHA_HPP_ENSVKDXU */

