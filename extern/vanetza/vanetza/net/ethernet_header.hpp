#ifndef ETHERNET_HEADER_HPP_
#define ETHERNET_HEADER_HPP_

#include <vanetza/common/byte_buffer.hpp>
#include <vanetza/common/byte_order.hpp>
#include <vanetza/net/mac_address.hpp>
#include <cstddef>

namespace vanetza
{

/**
 * Create a byte buffer containing an ethernet header
 * \param dest Destination MAC address
 * \param src Source MAC address
 * \param proto EtherType number
 */
ByteBuffer create_ethernet_header(const MacAddress& dest, const MacAddress& src, uint16be_t proto);

/**
 * Get length of ethernet header in bytes
 * \param header length in bytes
 */
constexpr std::size_t ethernet_header_length()
{
    return 2 * sizeof(MacAddress::octets) + sizeof(uint16_t);
}

} // namespace vanetza

#endif // ETHERNET_HEADER_HPP_

