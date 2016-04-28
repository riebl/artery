#ifndef COMMON_HEADER_HPP_SEFIWCT4
#define COMMON_HEADER_HPP_SEFIWCT4

#include <vanetza/common/bit_number.hpp>
#include <vanetza/geonet/header_type.hpp>
#include <vanetza/geonet/mib.hpp>
#include <vanetza/geonet/position_vector.hpp>
#include <vanetza/geonet/serialization.hpp>
#include <vanetza/geonet/traffic_class.hpp>
#include <cstdint>

namespace vanetza
{
namespace geonet
{

struct Area;
struct DataRequest;
struct ShbDataRequest;
class LocationTable;

enum class NextHeaderCommon
{
    ANY = 0, BTP_A = 1, BTP_B = 2, IPv6 = 3
};

struct CommonHeader
{
public:
    CommonHeader();
    CommonHeader(const MIB&);
    CommonHeader(const DataRequest&, const MIB&);
    CommonHeader(const ShbDataRequest&, const MIB&);

    static const std::size_t length_bytes = 8;

    NextHeaderCommon next_header; // 4 bit
    BitNumber<unsigned, 4> reserved1;
    HeaderType header_type;
    TrafficClass traffic_class;
    uint8_t flags; // Bit 0: itsGnIsMobile
    uint16_t payload; // number of octets following whole GeoNet header
    uint8_t maximum_hop_limit;
    uint8_t reserved2;
};

void serialize(const CommonHeader&, OutputArchive&);
void deserialize(CommonHeader&, InputArchive&);

} // namespace geonet
} // namespace vanetza

#endif /* COMMON_HEADER_HPP_SEFIWCT4 */

