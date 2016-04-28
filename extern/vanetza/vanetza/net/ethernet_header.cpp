#include "ethernet_header.hpp"
#include <algorithm>
#include <cassert>
#include <net/ethernet.h>

namespace vanetza
{

ByteBuffer create_ethernet_header(const MacAddress& dest, const MacAddress& src, uint16be_t proto)
{
    static_assert(sizeof(ethhdr) == ethernet_header_length(),
            "size of ethhdr and length of ethernet header have to be equal");
    ByteBuffer buffer(sizeof(ethhdr));
    ethhdr* hdr = buffer_cast<ethhdr>(buffer);
    assert(hdr != nullptr);

    std::copy(dest.octets.begin(), dest.octets.end(), hdr->h_dest);
    std::copy(src.octets.begin(), src.octets.end(), hdr->h_source);
    hdr->h_proto = proto.get();

    return buffer;
}

} // namespace vanetza

