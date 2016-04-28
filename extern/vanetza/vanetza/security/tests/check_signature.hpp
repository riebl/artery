#ifndef CHECK_SIGNATURE_HPP_7RESWTUO
#define CHECK_SIGNATURE_HPP_7RESWTUO

#include <vanetza/security/signature.hpp>

namespace vanetza
{
namespace security
{

void check(const EcdsaSignature&, const EcdsaSignature&);
void check(const EcdsaSignatureFuture&, const EcdsaSignatureFuture&);
void check(const Signature&, const Signature&);

/**
 * \brief create a random EcdsaSignature
 * \param seed the optional seed for the RNG
 * \return created signature
 */
EcdsaSignature create_random_ecdsa_signature(int seed = 0);

} // namespace security
} // namespace vanetza

#endif /* CHECK_SIGNATURE_HPP_7RESWTUO */
