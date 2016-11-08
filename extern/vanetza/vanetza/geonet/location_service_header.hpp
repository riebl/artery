#ifndef LOCATION_SERVICE_HEADER_HPP_0BWHQP7J
#define LOCATION_SERVICE_HEADER_HPP_0BWHQP7J

#include <vanetza/geonet/address.hpp>
#include <vanetza/geonet/position_vector.hpp>
#include <vanetza/geonet/sequence_number.hpp>

namespace vanetza
{
namespace geonet
{

struct LsRequestHeader
{
    static constexpr std::size_t length_bytes = 4 +
        LongPositionVector::length_bytes +
        Address::length_bytes;

    SequenceNumber sequence_number;
    uint16_t reserved;
    LongPositionVector source_position;
    Address request_addr;
};

struct LsReplyHeader
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

#endif /* LOCATION_SERVICE_HEADER_HPP_0BWHQP7J */

