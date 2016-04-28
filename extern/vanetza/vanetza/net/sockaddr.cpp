#include "sockaddr.hpp"
#include "mac_address.hpp"
#include <algorithm>
#include <cassert>
#include <net/ethernet.h>
#include <netpacket/packet.h>

namespace vanetza
{

void assign(sockaddr_ll& sockaddr, const MacAddress& mac)
{
    assert(ETHER_ADDR_LEN == mac.octets.size());
    std::copy_n(mac.octets.begin(), mac.octets.size(), sockaddr.sll_addr);
}

} // namespace vanetza
