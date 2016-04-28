#include <vanetza/geonet/basic_header.hpp>
#include <vanetza/geonet/data_request.hpp>
#include <vanetza/geonet/serialization.hpp>

namespace vanetza
{
namespace geonet
{

const std::size_t BasicHeader::length_bytes;

BasicHeader::BasicHeader() :
    version(0),
    next_header(NextHeaderBasic::ANY),
    reserved(0),
    hop_limit(0)
{
}

BasicHeader::BasicHeader(const MIB& mib) :
    version(mib.itsGnProtocolVersion),
    next_header(NextHeaderBasic::ANY),
    reserved(0),
    lifetime(mib.itsGnDefaultPacketLifetime),
    hop_limit(mib.itsGnDefaultHopLimit)
{
}

BasicHeader::BasicHeader(const DataRequest& request, const MIB& mib) :
    BasicHeader(mib)
{
    if (mib.itsGnSecurity) {
        next_header = NextHeaderBasic::SECURED;
    } else {
        next_header = NextHeaderBasic::COMMON;
    }

    lifetime = request.maximum_lifetime;
    hop_limit = request.max_hop_limit;
}

BasicHeader::BasicHeader(const ShbDataRequest& request, const MIB& mib) :
    BasicHeader(static_cast<const DataRequest&>(request), mib)
{
    hop_limit = 1;
}

void serialize(const BasicHeader& hdr, OutputArchive& ar)
{
    uint8_t versionAndNextHeader = hdr.version.raw();
    versionAndNextHeader <<= 4;
    versionAndNextHeader |= static_cast<uint8_t>(hdr.next_header) & 0x0f;
    serialize(host_cast(versionAndNextHeader), ar);
    serialize(host_cast(hdr.reserved), ar);
    serialize(hdr.lifetime, ar);
    serialize(host_cast(hdr.hop_limit), ar);
}

void deserialize(BasicHeader& hdr, InputArchive& ar)
{
    uint8_t versionAndNextHeader;
    deserialize(versionAndNextHeader, ar);
    hdr.version = versionAndNextHeader >> 4;
    hdr.next_header = static_cast<NextHeaderBasic>(versionAndNextHeader & 0x0f);
    deserialize(hdr.reserved, ar);
    deserialize(hdr.lifetime, ar);
    deserialize(hdr.hop_limit, ar);
}

} // namespace geonet
} // namespace vanetza
