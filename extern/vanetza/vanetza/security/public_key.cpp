#include <vanetza/security/exception.hpp>
#include <vanetza/security/public_key.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>

namespace vanetza
{
namespace security
{

PublicKeyAlgorithm get_type(const PublicKey& key)
{
    struct public_key_visitor : public boost::static_visitor<PublicKeyAlgorithm>
    {
        PublicKeyAlgorithm operator()(const ecdsa_nistp256_with_sha256& ecdsa)
        {
            return PublicKeyAlgorithm::Ecdsa_Nistp256_With_Sha256;
        }
        PublicKeyAlgorithm operator()(const ecies_nistp256& ecies)
        {
            return PublicKeyAlgorithm::Ecies_Nistp256;
        }
    };

    public_key_visitor visit;
    return boost::apply_visitor(visit, key);
}

void serialize(OutputArchive& ar, const PublicKey& key)
{
    struct public_key_visitor : public boost::static_visitor<>
    {
        public_key_visitor(OutputArchive& ar, PublicKeyAlgorithm algo) :
            m_archive(ar), m_algo(algo)
        {
        }
        void operator()(const ecdsa_nistp256_with_sha256& ecdsa)
        {
            serialize(m_archive, ecdsa.public_key, m_algo);
        }
        void operator()(const ecies_nistp256& ecies)
        {
            serialize(m_archive, ecies.supported_symm_alg);
            serialize(m_archive, ecies.public_key, m_algo);
        }
        OutputArchive& m_archive;
        PublicKeyAlgorithm m_algo;
    };

    PublicKeyAlgorithm type = get_type(key);
    serialize(ar, type);
    public_key_visitor visit(ar, type);
    boost::apply_visitor(visit, key);
}

std::size_t field_size(PublicKeyAlgorithm algo)
{
    size_t size = 0;
    switch (algo) {
        case PublicKeyAlgorithm::Ecdsa_Nistp256_With_Sha256:
            size = 32;
            break;
        case PublicKeyAlgorithm::Ecies_Nistp256:
            size = 32;
            break;
    }
    return size;
}

std::size_t field_size(SymmetricAlgorithm algo)
{
    size_t size = 0;
    switch (algo) {
        case SymmetricAlgorithm::Aes128_Ccm:
            size = 16;
            break;
        default:
            throw deserialization_error("Unknown SymmetricAlgorithm");
            break;
    }
    return size;
}

size_t deserialize(InputArchive& ar, PublicKey& key)
{
    PublicKeyAlgorithm type;
    deserialize(ar, type);
    switch (type) {
        case PublicKeyAlgorithm::Ecdsa_Nistp256_With_Sha256: {
            ecdsa_nistp256_with_sha256 ecdsa;
            deserialize(ar, ecdsa.public_key, PublicKeyAlgorithm::Ecdsa_Nistp256_With_Sha256);
            key = ecdsa;
            break;
        }
        case PublicKeyAlgorithm::Ecies_Nistp256: {
            ecies_nistp256 ecies;
            deserialize(ar, ecies.supported_symm_alg);
            deserialize(ar, ecies.public_key, PublicKeyAlgorithm::Ecies_Nistp256);
            key = ecies;
            break;
        }
        default:
            throw deserialization_error("Unknown PublicKeyAlgorithm");
            break;
    }
    return get_size(key);
}

size_t get_size(const PublicKey& key)
{
    size_t size = sizeof(PublicKeyAlgorithm);
    struct publicKey_visitor : public boost::static_visitor<size_t>
    {
        size_t operator()(ecdsa_nistp256_with_sha256 key)
        {
            return get_size(key.public_key);
        }
        size_t operator()(ecies_nistp256 key)
        {
            return get_size(key.public_key) + sizeof(key.supported_symm_alg);
        }
    };
    publicKey_visitor visit;
    size += boost::apply_visitor(visit, key);
    return size;
}

} // namespace security
} // namespace vanetza
