#ifndef GBC_HEADER_HPP_AWYN1M7E
#define GBC_HEADER_HPP_AWYN1M7E

#include <vanetza/geonet/gbc_gac_header.hpp>
#include <vanetza/geonet/header_type.hpp>
#include <vanetza/geonet/serialization.hpp>

namespace vanetza
{
namespace geonet
{

struct GeoBroadcastHeader : public detail::GbcGacHeader
{
public:
    using detail::GbcGacHeader::destination;
    Area destination(HeaderType) const;
};

void serialize(const GeoBroadcastHeader&, OutputArchive&);
void deserialize(GeoBroadcastHeader&, InputArchive&);

} // namespace geonet
} // namespace vanetza

#endif /* GBC_HEADER_HPP_AWYN1M7E */

