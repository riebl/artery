#ifndef PACKET_BUFFER_HPP_U97KIBQC
#define PACKET_BUFFER_HPP_U97KIBQC

#include <vanetza/geonet/lifetime.hpp>
#include <vanetza/geonet/next_hop.hpp>
#include <vanetza/geonet/packet.hpp>
#include <vanetza/geonet/pdu.hpp>
#include <vanetza/geonet/timestamp.hpp>
#include <vanetza/net/mac_address.hpp>
#include <functional>
#include <list>
#include <memory>

namespace vanetza
{
namespace geonet
{
namespace packet_buffer
{

class Expiry
{
public:
    Expiry(Timestamp now, Lifetime lifetime);
    bool is_expired(Timestamp now) const;
    Timestamp buffered_since() const { return m_buffered_since; }
    Timestamp expires_at() const { return m_expires_at; }

private:
    Timestamp m_buffered_since;
    Timestamp m_expires_at;
};

class Data
{
public:
    /**
     * Length of packet data
     * \return length in bytes
     */
    virtual std::size_t length() const = 0;

    /**
     * Lifetime associated with data
     * \return Lifetime reference
     */
    virtual Lifetime& lifetime() = 0;

    /**
     * Flush data
     * \note length() and lifetime() should not be called afterwards!
     * \return next hop data
     */
    virtual NextHop flush() = 0;

    virtual ~Data() {}
};

} // namespace packet_buffer


/**
 * PacketBuffer with bounded capacity, packet expiry and head-drop
 */
class PacketBuffer
{
public:
    typedef std::unique_ptr<packet_buffer::Data> data_ptr;

    /**
     * Create PacketBuffer with given capacity
     * \param capacity Buffer can store this many bytes
     */
    PacketBuffer(std::size_t capacity);

    /**
     * Push one packet into buffer
     * \param packet Packet data
     * \param t Current time
     * \return true if packet has been pushed successfully
     */
    bool push(data_ptr packet, Timestamp t);

    /**
     * Flush packets from buffer. Expired packets are dropped.
     * \note Some packets might remain in buffer
     * \param t Current time
     * \return list of ready-to-send packets
     */
    std::list<NextHop> flush(Timestamp t);

private:
    typedef packet_buffer::Expiry expiry_type;
    typedef std::tuple<expiry_type, data_ptr> node_type;

    std::size_t free() const { return m_capacity - m_stored; }
    std::size_t capacity() const { return m_capacity; }

    /**
     * Push one new element into buffer list
     * \note capacity is not checked by this method, has to be done manually
     * \param expiry Expiry data
     * \param packet Packet data
     */
    void push(expiry_type&& expiry, data_ptr packet);

    /**
     * Drop current head element
     * \return true if head element was dropped
     */
    bool drop_head();

    /**
     * Drop all packets with expired timestamp
     * \param t current time
     */
    void drop_expired(Timestamp t);

    /**
     * Drop as many packets as required to store given number of bytes.
     * Packets at the head of the list are dropped first.
     * \param bytes require #bytes free capacity
     * \return true if there is enough capacity left for #bytes
     */
    bool drop(std::size_t bytes);

    std::list<node_type> m_nodes;
    std::size_t m_capacity;
    std::size_t m_stored;
};

} // namespace geonet
} // namespace vanetza

#endif /* PACKET_BUFFER_HPP_U97KIBQC */

