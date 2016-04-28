#include <vanetza/security/exception.hpp>
#include <vanetza/security/trailer_field.hpp>
#include <boost/variant/get.hpp>
#include <cassert>

namespace vanetza
{
namespace security
{

TrailerFieldType get_type(const TrailerField& field)
{
    struct trailerFieldVisitor : public boost::static_visitor<TrailerFieldType>
    {
        TrailerFieldType operator()(const Signature& sig)
        {
            return TrailerFieldType::Signature;
        }
    };
    trailerFieldVisitor visit;
    return boost::apply_visitor(visit, field);
}

size_t get_size(const TrailerField& field)
{
    size_t size = sizeof(TrailerFieldType);
    struct trailerFieldVisitor : public boost::static_visitor<size_t>
    {
        size_t operator()(const Signature& sig)
        {
            return get_size(sig);
        }
    };
    trailerFieldVisitor visit;
    size += boost::apply_visitor(visit, field);
    return size;
}

void serialize(OutputArchive& ar, const TrailerField& field)
{
    struct trailer_visitor : public boost::static_visitor<>
    {
        trailer_visitor(OutputArchive& ar) :
            m_archive(ar)
        {
        }
        void operator()(const Signature& sig)
        {
            serialize(m_archive, sig);
        }
        OutputArchive& m_archive;
    };
    TrailerFieldType type = get_type(field);
    serialize(ar, type);
    trailer_visitor visitor(ar);
    boost::apply_visitor(visitor, field);
}

size_t deserialize(InputArchive& ar, TrailerField& field)
{
    size_t size = 0;
    TrailerFieldType type;
    deserialize(ar, type);
    size += sizeof(TrailerFieldType);
    switch (type) {
        case TrailerFieldType::Signature: {
            Signature sig;
            size += deserialize(ar, sig);
            field = sig;
            break;
        }
        default:
            throw deserialization_error("Unknown TrailerFieldType");
    }
    return size;
}

boost::optional<ByteBuffer> extract_signature_buffer(const TrailerField& trailer_field)
{
    assert(TrailerFieldType::Signature == get_type(trailer_field));
    assert(PublicKeyAlgorithm::Ecdsa_Nistp256_With_Sha256 == get_type(boost::get<Signature>(trailer_field)));

    return extract_signature_buffer(boost::get<Signature>(trailer_field));
}

} // namespace security
} // namespace vanetza
