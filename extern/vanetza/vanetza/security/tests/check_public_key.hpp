#ifndef CHECK_PUBLIC_KEY_HPP_3HUSMPTE
#define CHECK_PUBLIC_KEY_HPP_3HUSMPTE

#include <vanetza/security/public_key.hpp>

namespace vanetza
{
namespace security
{

void check(const ecdsa_nistp256_with_sha256&, const ecdsa_nistp256_with_sha256&);
void check(const ecies_nistp256&, const ecies_nistp256&);
void check(const PublicKey&, const PublicKey&);

PublicKey create_random_public_key(int seed = 0);

} // namespace security
} // namespace vanetza

#endif /* CHECK_PUBLIC_KEY_HPP_3HUSMPTE */

