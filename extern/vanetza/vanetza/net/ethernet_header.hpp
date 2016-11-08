#ifndef ETHERNET_HEADER_HPP_
#define ETHERNET_HEADER_HPP_

#include <vanetza/common/byte_buffer.hpp>
#include <vanetza/common/byte_order.hpp>
#include <vanetza/net/mac_address.hpp>
#include <cstddef>

namespace vanetza
{

/**
 * Get length of ethernet header in bytes
 * \param header length in bytes
 */
constexpr std::size_t ethernet_header_length()
{
    return 2 * sizeof(MacAddress::octets) + sizeof(uint16_t);
}

/**
 * Link-level header of type Ethernet II
 *
 * EthernetHeader is suitable for Linux raw packet sockets, see man 7 packet
 */
class EthernetHeader
{
public:
    using EtherType = uint16be_t;
    static constexpr std::size_t length_bytes = ethernet_header_length();

    MacAddress destination;
    MacAddress source;
    EtherType type;
};

/**
 * Create a byte buffer containing an ethernet header
 * \param dest Destination MAC address
 * \param src Source MAC address
 * \param proto EtherType number
 */
ByteBuffer create_ethernet_header(const MacAddress& dest, const MacAddress& src, uint16be_t proto);
ByteBuffer create_ethernet_header(const EthernetHeader&);

/**
 * Parse ethernet header from byte buffer
 * \param buffer byte buffer (throws exception if too short)
 * \return ethernet header
 */
EthernetHeader decode_ethernet_header(const ByteBuffer&);
EthernetHeader decode_ethernet_header(ByteBuffer::const_iterator, ByteBuffer::const_iterator);

} // namespace vanetza

#endif // ETHERNET_HEADER_HPP_

