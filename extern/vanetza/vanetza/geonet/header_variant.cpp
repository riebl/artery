#include <vanetza/geonet/header_variant.hpp>

namespace boost
{

using namespace vanetza::geonet;

class HeaderVariantVisitor : public boost::static_visitor<>
{
public:
    HeaderVariantVisitor(OutputArchive& ar) :
        m_archive(ar)
    {
    }

    template<typename T>
    void operator()(const T& header)
    {
        serialize(header, m_archive);
    }

private:
    OutputArchive& m_archive;
};

void serialize(const HeaderVariant& header, OutputArchive& ar)
{
    HeaderVariantVisitor visit(ar);
    boost::apply_visitor(visit, header);
}

void serialize(const HeaderRefVariant& header, OutputArchive& ar)
{
    HeaderVariantVisitor visitor(ar);
    boost::apply_visitor(visitor, header);
}

void serialize(const HeaderConstRefVariant& header, OutputArchive& ar)
{
    HeaderVariantVisitor visitor(ar);
    boost::apply_visitor(visitor, header);
}

} // namespace boost


namespace vanetza
{
namespace geonet
{

class HeaderVariantLengthVisitor : public boost::static_visitor<std::size_t>
{
public:
    template<typename T>
    std::size_t operator()(const T& header)
    {
        return T::length_bytes;
    }
};

std::size_t get_length(const HeaderVariant& header)
{
    HeaderVariantLengthVisitor visit;
    return boost::apply_visitor(visit, header);
}

std::size_t get_length(const HeaderRefVariant& header)
{
    HeaderVariantLengthVisitor visitor;
    return boost::apply_visitor(visitor, header);
}

std::size_t get_length(const HeaderConstRefVariant& header)
{
    HeaderVariantLengthVisitor visitor;
    return boost::apply_visitor(visitor, header);
}

} // namespace geonet
} // namespace vanetza
