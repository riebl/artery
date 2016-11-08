#include "ethernet_header.hpp"
#include <algorithm>
#include <cassert>
#include <stdexcept>

namespace vanetza
{

ByteBuffer create_ethernet_header(const MacAddress& dest, const MacAddress& src, uint16be_t proto)
{
    ByteBuffer buffer;
    buffer.reserve(EthernetHeader::length_bytes);
    auto inserter = std::back_inserter(buffer);
    std::copy(dest.octets.begin(), dest.octets.end(), inserter);
    std::copy(src.octets.begin(), src.octets.end(), inserter);
    uint16_t host_proto = proto.host();
    inserter = (host_proto >> 8) & 0xff;
    inserter = host_proto & 0xff;
    assert(buffer.size() == EthernetHeader::length_bytes);
    return buffer;
}

ByteBuffer create_ethernet_header(const EthernetHeader& hdr)
{
    return create_ethernet_header(hdr.destination, hdr.source, hdr.type);
}

EthernetHeader decode_ethernet_header(ByteBuffer::const_iterator begin, ByteBuffer::const_iterator end)
{
    EthernetHeader hdr;
    const std::size_t buflen = std::distance(begin, end);
    if (buflen < EthernetHeader::length_bytes) {
        throw std::runtime_error("buffer is too short for EthernetHeader decoding");
    } else {
        std::copy_n(begin, MacAddress::length_bytes, hdr.destination.octets.begin());
        begin += MacAddress::length_bytes;
        std::copy_n(begin, MacAddress::length_bytes, hdr.source.octets.begin());
        begin += MacAddress::length_bytes;
        uint16_t proto = (begin[0] << 8) | begin[1];
        hdr.type = host_cast(proto);
    }
    return hdr;
}

EthernetHeader decode_ethernet_header(const ByteBuffer& buf)
{
    return decode_ethernet_header(buf.begin(), buf.end());
}

} // namespace vanetza

