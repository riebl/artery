#ifndef SHB_HEADER_HPP_MRLDRPNK
#define SHB_HEADER_HPP_MRLDRPNK

#include <vanetza/geonet/position_vector.hpp>
#include <cstdint>

namespace vanetza
{
namespace geonet
{

struct ShbHeader
{
    ShbHeader();

    static const std::size_t length_bytes = 4 + LongPositionVector::length_bytes;

    LongPositionVector source_position;
    uint32_t reserved; // ITS-G5 uses it for DCC related information
};

void serialize(const ShbHeader&, OutputArchive&);
void deserialize(ShbHeader&, InputArchive&);

} // namespace geonet
} // namespace vanetza

#endif /* SHB_HEADER_HPP_MRLDRPNK */

