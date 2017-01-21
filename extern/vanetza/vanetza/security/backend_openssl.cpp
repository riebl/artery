#include <vanetza/security/backend_openssl.hpp>
#include <vanetza/security/openssl_wrapper.hpp>
#include <vanetza/security/public_key.hpp>
#include <vanetza/security/signature.hpp>
#include <openssl/bn.h>
#include <openssl/ecdsa.h>
#include <openssl/obj_mac.h>
#include <openssl/sha.h>
#include <cassert>

namespace vanetza
{
namespace security
{

BackendOpenSsl::BackendOpenSsl()
{
#if OPENSSL_API_COMPAT < 0x10100000L
    ERR_load_crypto_strings();
#else
    OPENSSL_init_crypto(OPENSSL_INIT_LOAD_CRYPTO_STRINGS, nullptr);
#endif
}

EcdsaSignature BackendOpenSsl::sign_data(const ecdsa256::PrivateKey& key, const ByteBuffer& data)
{
    auto priv_key = internal_private_key(key);
    auto digest = calculate_digest(data);

    // sign message data represented by the digest
    openssl::Signature signature { ECDSA_do_sign(digest.data(), digest.size(), priv_key) };
#if OPENSSL_API_COMPAT < 0x10100000L
    const BIGNUM* sig_r = signature->r;
    const BIGNUM* sig_s = signature->s;
#else
    const BIGNUM* sig_r = nullptr;
    const BIGNUM* sig_s = nullptr;
    ECDSA_SIG_get0(signature, &sig_r, &sig_s);
#endif

    EcdsaSignature ecdsa_signature;
    X_Coordinate_Only coordinate;

    if (sig_r && sig_s) {
        const size_t len = field_size(PublicKeyAlgorithm::Ecdsa_Nistp256_With_Sha256);

        const auto num_bytes_s = BN_num_bytes(sig_s);
        assert(len >= static_cast<size_t>(num_bytes_s));
        ecdsa_signature.s.resize(len, 0x00);
        BN_bn2bin(sig_s, ecdsa_signature.s.data() + len - num_bytes_s);

        const auto num_bytes_r = BN_num_bytes(sig_r);
        assert(len >= static_cast<size_t>(num_bytes_r));
        coordinate.x.resize(len, 0x00);
        BN_bn2bin(sig_r, coordinate.x.data() + len - num_bytes_r);
    } else {
        throw openssl::Exception();
    }

    ecdsa_signature.R = std::move(coordinate);
    return ecdsa_signature;
}

bool BackendOpenSsl::verify_data(const ecdsa256::PublicKey& key, const ByteBuffer& data, const EcdsaSignature& sig)
{
    auto digest = calculate_digest(data);
    auto pub = internal_public_key(key);
    openssl::Signature signature(sig);

    return (ECDSA_do_verify(digest.data(), digest.size(), signature, pub) == 1);
}

std::array<uint8_t, 32> BackendOpenSsl::calculate_digest(const ByteBuffer& data) const
{
    static_assert(SHA256_DIGEST_LENGTH == 32, "Unexpected length of SHA256 digest");

    std::array<uint8_t, 32> digest;
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, data.data(), data.size());
    SHA256_Final(digest.data(), &ctx);
    return digest;
}

openssl::Key BackendOpenSsl::internal_private_key(const ecdsa256::PrivateKey& generic) const
{
    openssl::Key key(NID_X9_62_prime256v1);
    openssl::BigNumber prv(generic.key);
    EC_KEY_set_private_key(key, prv);

    // OpenSSL requires public key, so we recreate it from private key
    openssl::BigNumberContext ctx;
    const EC_GROUP* group = EC_KEY_get0_group(key);
    openssl::Point pub(group);
    openssl::check(EC_POINT_mul(group, pub, prv, nullptr, nullptr, ctx));
    EC_KEY_set_public_key(key, pub);

    openssl::check(EC_KEY_check_key(key));
    return key;
}

openssl::Key BackendOpenSsl::internal_public_key(const ecdsa256::PublicKey& generic) const
{
    openssl::Key key(NID_X9_62_prime256v1);
    openssl::BigNumber x(generic.x);
    openssl::BigNumber y(generic.y);
    EC_KEY_set_public_key_affine_coordinates(key, x, y);

    openssl::check(EC_KEY_check_key(key));
    return key;
}

} // namespace security
} // namespace vanetza
