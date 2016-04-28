#include <vanetza/security/ecc_point.hpp>
#include <vanetza/security/exception.hpp>
#include <vanetza/security/public_key.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>
#include <cassert>

namespace vanetza
{
namespace security
{

size_t get_size(const EccPoint& point)
{
    size_t size = sizeof(EccPointType);
    struct ecc_point_visitor : public boost::static_visitor<size_t>
    {
        size_t operator()(X_Coordinate_Only coord)
        {
            return coord.x.size();
        }
        size_t operator()(Compressed_Lsb_Y_0 coord)
        {
            return coord.x.size();
        }
        size_t operator()(Compressed_Lsb_Y_1 coord)
        {
            return coord.x.size();
        }
        size_t operator()(Uncompressed coord)
        {
            return coord.x.size() + coord.y.size();
        }
    };

    ecc_point_visitor visit;
    boost::apply_visitor(visit, point);

    size += boost::apply_visitor(visit, point);
    return size;
}

EccPointType get_type(const EccPoint& point)
{
    struct ecc_point_visitor : public boost::static_visitor<EccPointType>
    {
        EccPointType operator()(X_Coordinate_Only coord)
        {
            return EccPointType::X_Coordinate_Only;
        }
        EccPointType operator()(Compressed_Lsb_Y_0 coord)
        {
            return EccPointType::Compressed_Lsb_Y_0;
        }
        EccPointType operator()(Compressed_Lsb_Y_1 coord)
        {
            return EccPointType::Compressed_Lsb_Y_1;
        }
        EccPointType operator()(Uncompressed coord)
        {
            return EccPointType::Uncompressed;
        }
    };

    ecc_point_visitor visit;
    return boost::apply_visitor(visit, point);
}

void serialize(OutputArchive& ar, const EccPoint& point, PublicKeyAlgorithm algo)
{
    struct ecc_point_visitor : public boost::static_visitor<>
    {
        ecc_point_visitor(OutputArchive& ar, PublicKeyAlgorithm algo) :
            m_archive(ar), m_algo(algo)
        {
        }
        void operator()(X_Coordinate_Only coord)
        {
            assert(coord.x.size() == field_size(m_algo));
            for (auto byte : coord.x) {
                m_archive << byte;
            }
        }
        void operator()(Compressed_Lsb_Y_0 coord)
        {
            assert(coord.x.size() == field_size(m_algo));
            for (auto byte : coord.x) {
                m_archive << byte;
            }
        }
        void operator()(Compressed_Lsb_Y_1 coord)
        {
            assert(coord.x.size() == field_size(m_algo));
            for (auto byte : coord.x) {
                m_archive << byte;
            }
        }
        void operator()(Uncompressed coord)
        {
            assert(coord.x.size() == field_size(m_algo));
            assert(coord.y.size() == field_size(m_algo));
            for (auto byte : coord.x) {
                m_archive << byte;
            }
            for (auto byte : coord.y) {
                m_archive << byte;
            }
        }
        OutputArchive& m_archive;
        PublicKeyAlgorithm m_algo;
    };

    EccPointType type = get_type(point);
    serialize(ar, type);
    ecc_point_visitor visit(ar, algo);
    boost::apply_visitor(visit, point);
}

void deserialize(InputArchive& ar, EccPoint& point, PublicKeyAlgorithm algo)
{
    size_t size = field_size(algo);
    uint8_t elem;
    EccPointType type;
    deserialize(ar, type);
    switch (type) {
        case EccPointType::X_Coordinate_Only: {
            X_Coordinate_Only coord;
            for (size_t c = 0; c < size; c++) {
                ar >> elem;
                coord.x.push_back(elem);
            }
            point = coord;
            break;
        }
        case EccPointType::Compressed_Lsb_Y_0: {
            Compressed_Lsb_Y_0 coord;
            for (size_t c = 0; c < size; c++) {
                ar >> elem;
                coord.x.push_back(elem);
            }
            point = coord;
            break;
        }
        case EccPointType::Compressed_Lsb_Y_1: {
            Compressed_Lsb_Y_1 coord;
            for (size_t c = 0; c < size; c++) {
                ar >> elem;
                coord.x.push_back(elem);
            }
            point = coord;
            break;
        }
        case EccPointType::Uncompressed: {
            Uncompressed coord;
            for (size_t c = 0; c < size; c++) {
                ar >> elem;
                coord.x.push_back(elem);
            }
            for (size_t c = 0; c < size; c++) {
                ar >> elem;
                coord.y.push_back(elem);
            }
            point = coord;
            break;
        }
        default:
            throw deserialization_error("Unknown EccPointType");
    }
}

class EccPointVisitor : public boost::static_visitor<ByteBuffer>
{
public:
    template<typename T>
    ByteBuffer operator()(const T& point)
    {
        return point.x;
    }
};

ByteBuffer convert_for_signing(const EccPoint& ecc_point)
{
    EccPointVisitor visit;
    return boost::apply_visitor(visit, ecc_point);
}

} // namespace security
} // namespace vanetza
