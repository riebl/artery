#ifndef BACKEND_HPP_ZMRDTY2O
#define BACKEND_HPP_ZMRDTY2O

#include <vanetza/common/byte_buffer.hpp>
#include <vanetza/security/ecdsa256.hpp>
#include <vanetza/security/signature.hpp>
#include <memory>
#include <string>

namespace vanetza
{
namespace security
{

/**
 * Interface to cryptographic features
 */
class Backend
{
public:
    /**
     * \brief calculate signature for given data and private key
     *
     * \param private_key Secret private key
     * \param data buffer with plaintext data
     * \return calculated signature
     */
    virtual EcdsaSignature sign_data(const ecdsa256::PrivateKey& private_key, const ByteBuffer& data) = 0;

    /**
     * \brief try to verify data using public key and signature
     *
     * \param public_key Public key
     * \param data plaintext
     * \param sig signature of data
     * \return true if the data could be verified
     */
    virtual bool verify_data(const ecdsa256::PublicKey& public_key, const ByteBuffer& data, const EcdsaSignature& sig) = 0;

    virtual ~Backend() = default;
};

/**
 * \brief create a backend instance
 *
 * A backend named "default" is guaranteed not to return a nullptr.
 * However, it might be a dummy backend.
 *
 * \param name identifying name of backend implementation
 * \return backend instance (if available) or nullptr
 */
std::unique_ptr<Backend> create_backend(const std::string& name);

} // namespace security
} // namespace vanetza

#endif /* BACKEND_HPP_ZMRDTY2O */

