#ifndef GUC_HEADER_HPP_UEGE3IFW
#define GUC_HEADER_HPP_UEGE3IFW

#include <vanezta/geonet/position_vector.hpp>
#include <vanetza/geonet/sequence_number.hpp>
#include <cstddef>

namespace vanetza
{
namespace geonet
{

struct GeoUnicastHeader
{
    static constexpr std::size_t length_bytes = 4 +
        LongPositionVector::length_bytes +
        ShortPositionVector::length_bytes;

    SequenceNumber sequence_number;
    uint16_t reserved;
    LongPositionVector source_position;
    ShortPositionVector destination_position;
};

} // namespace geonet
} // namespace vanetza

#endif /* GUC_HEADER_HPP_UEGE3IFW */

