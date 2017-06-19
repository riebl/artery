#ifndef BACKEND_OPENSSL_HPP_CRRV8DCH
#define BACKEND_OPENSSL_HPP_CRRV8DCH

#include <vanetza/security/backend.hpp>
#include <array>
#include <cstdint>

namespace vanetza
{
namespace security
{

// forward declaration
namespace openssl
{
class Key;
} // namespace openssl


/**
 * \brief Backend implementation based on OpenSSL
 */
class BackendOpenSsl : public Backend
{
public:
    static constexpr auto backend_name = "OpenSSL";

    BackendOpenSsl();

    /// \see Backend::sign_data
    EcdsaSignature sign_data(const ecdsa256::PrivateKey& private_key, const ByteBuffer& data_buffer) override;

    /// \see Backend::verify_data
    bool verify_data(const ecdsa256::PublicKey& public_key, const ByteBuffer& data, const EcdsaSignature& sig) override;

private:
    /// calculate SHA256 digest of data buffer
    std::array<uint8_t, 32> calculate_digest(const ByteBuffer& data) const;

    /// convert to internal format of private key
    openssl::Key internal_private_key(const ecdsa256::PrivateKey&) const;

    /// convert to internal format of public key
    openssl::Key internal_public_key(const ecdsa256::PublicKey&) const;
};

} // namespace security
} // namespace vanetza

#endif /* BACKEND_OPENSSL_HPP_CRRV8DCH */
