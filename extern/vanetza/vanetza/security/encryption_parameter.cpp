#include <vanetza/security/encryption_parameter.hpp>
#include <vanetza/security/exception.hpp>
#include <vanetza/security/public_key.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>

namespace vanetza
{
namespace security
{

SymmetricAlgorithm get_type(const EncryptionParameter& param)
{
    struct Encryption_visitor : public boost::static_visitor<SymmetricAlgorithm>
    {
        SymmetricAlgorithm operator()(const Nonce& nonce)
        {
            return SymmetricAlgorithm::Aes128_Ccm;
        }
    };

    Encryption_visitor visit;
    return boost::apply_visitor(visit, param);
}

void serialize(OutputArchive& ar, const EncryptionParameter& param)
{
    struct Encryption_visitor : public boost::static_visitor<>
    {
        Encryption_visitor(OutputArchive& ar) :
            m_archive(ar)
        {
        }
        void operator()(const Nonce& nonce)
        {
            for (auto& byte : nonce) {
                m_archive << byte;
            }
        }
        OutputArchive& m_archive;
    };

    SymmetricAlgorithm algo = get_type(param);
    serialize(ar, algo);
    Encryption_visitor visit(ar);
    boost::apply_visitor(visit, param);
}

size_t get_size(const EncryptionParameter& param)
{
    size_t size = sizeof(SymmetricAlgorithm);
    struct Encryption_visitor : public boost::static_visitor<size_t>
    {
        size_t operator()(const Nonce& nonce)
        {
            return nonce.size();
        }
    };

    Encryption_visitor visit;
    size += boost::apply_visitor(visit, param);
    return size;
}

size_t deserialize(InputArchive& ar, EncryptionParameter& param)
{
    SymmetricAlgorithm algo;
    deserialize(ar, algo);
    switch (algo) {
        case SymmetricAlgorithm::Aes128_Ccm: {
            Nonce nonce;
            for (size_t s = 0; s < nonce.size(); s++) {
                ar >> nonce[s];
            }
            param = nonce;
            break;
        }
        default:
            throw deserialization_error("Unknown Symmetric Algorithm");
            break;
    }
    return get_size(param);
}

} // namespace security
} // namespace vanetza
