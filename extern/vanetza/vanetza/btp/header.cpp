#include "header.hpp"
#include <vanetza/common/serialization.hpp>

namespace vanetza
{
namespace btp
{

using vanetza::serialize;
using vanetza::deserialize;

const std::size_t HeaderA::length_bytes;
const std::size_t HeaderB::length_bytes;

void serialize(OutputArchive& ar, const HeaderB& hdr)
{
    serialize(ar, hdr.destination_port);
    serialize(ar, hdr.destination_port_info);
}

void deserialize(InputArchive& ar, HeaderB& hdr)
{
    deserialize(ar, hdr.destination_port);
    deserialize(ar, hdr.destination_port_info);
}

} // namespace btp
} // namespace vanetza

