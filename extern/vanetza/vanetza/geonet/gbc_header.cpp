#include "gbc_header.hpp"
#include "areas.hpp"
#include <stdexcept>

namespace vanetza
{
namespace geonet
{

Area GeoBroadcastHeader::destination(HeaderType ht) const
{
    decltype(Area::shape) shape;
    switch (ht) {
        case HeaderType::GEOBROADCAST_CIRCLE:
            shape = Circle();
            break;
        case HeaderType::GEOBROADCAST_RECT:
            shape = Rectangle();
            break;
        case HeaderType::GEOBROADCAST_ELIP:
            shape = Ellipse();
            break;
        default:
            throw std::runtime_error("Invalid GBC header type");
            break;
    }

    return detail::GbcGacHeader::destination(shape);
}

void serialize(const GeoBroadcastHeader& header, OutputArchive& ar)
{
    detail::serialize(header, ar);
}

void deserialize(GeoBroadcastHeader& header, InputArchive& ar)
{
    detail::deserialize(header, ar);
}

} // namespace geonet
} // namespace vanetza
