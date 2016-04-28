#include "common_header.hpp"
#include "data_request.hpp"
#include "serialization.hpp"
#include <stdexcept>

namespace vanetza
{
namespace geonet
{

const std::size_t CommonHeader::length_bytes;

CommonHeader::CommonHeader() :
    next_header(NextHeaderCommon::ANY),
    reserved1(0),
    header_type(HeaderType::ANY),
    flags(0),
    payload(0),
    maximum_hop_limit(0),
    reserved2(0)
{
}

CommonHeader::CommonHeader(const MIB& mib) :
    next_header(NextHeaderCommon::ANY),
    reserved1(0),
    header_type(HeaderType::ANY),
    traffic_class(mib.itsGnDefaultTrafficClass),
    flags(mib.itsGnIsMobile ? 0x80 : 0x00),
    payload(0),
    maximum_hop_limit(mib.itsGnDefaultHopLimit),
    reserved2(0)
{
}

CommonHeader::CommonHeader(const DataRequest& request, const MIB& mib) :
    CommonHeader(mib)
{
    switch (request.upper_protocol) {
        case UpperProtocol::BTP_A:
            next_header = NextHeaderCommon::BTP_A;
            break;
        case UpperProtocol::BTP_B:
            next_header = NextHeaderCommon::BTP_B;
            break;
        case UpperProtocol::IPv6:
            next_header = NextHeaderCommon::IPv6;
            break;
        default:
            throw std::runtime_error("Unhandled upper protocol");
            break;
    }

    traffic_class = request.traffic_class;
    maximum_hop_limit = request.max_hop_limit;
}

CommonHeader::CommonHeader(const ShbDataRequest& request, const MIB& mib) :
    CommonHeader(static_cast<const DataRequest&>(request), mib)
{
    header_type = HeaderType::TSB_SINGLE_HOP;
    maximum_hop_limit = 1;
}

void serialize(const CommonHeader& hdr, OutputArchive& ar)
{
    uint8_t nextHeaderAndReserved = static_cast<uint8_t>(hdr.next_header);
    nextHeaderAndReserved <<= 4;
    nextHeaderAndReserved |= hdr.reserved1.raw();
    serialize(host_cast(nextHeaderAndReserved), ar);
    serialize(host_cast(static_cast<std::underlying_type<HeaderType>::type>(hdr.header_type)), ar);
    serialize(hdr.traffic_class, ar);
    serialize(host_cast(hdr.flags), ar);
    serialize(host_cast(hdr.payload), ar);
    serialize(host_cast(hdr.maximum_hop_limit), ar);
    serialize(host_cast(hdr.reserved2), ar);
}

void deserialize(CommonHeader& hdr, InputArchive& ar)
{
    uint8_t nextHeaderAndReserved;
    deserialize(nextHeaderAndReserved, ar);
    hdr.next_header = static_cast<NextHeaderCommon>(nextHeaderAndReserved >> 4);
    hdr.reserved1 = nextHeaderAndReserved & 0x0f;
    typename std::underlying_type<HeaderType>::type headerType;
    deserialize(headerType, ar);
    hdr.header_type = static_cast<HeaderType>(headerType);
    deserialize(hdr.traffic_class, ar);
    deserialize(hdr.flags, ar);
    deserialize(hdr.payload, ar);
    deserialize(hdr.maximum_hop_limit, ar);
    deserialize(hdr.reserved2, ar);
}

} // namespace geonet
} // namespace vanetza

