#ifndef COHESIVE_PACKET_HPP_VG2XKSCV
#define COHESIVE_PACKET_HPP_VG2XKSCV

#include <vanetza/common/byte_buffer.hpp>
#include <vanetza/net/osi_layer.hpp>
#include <boost/range/iterator_range.hpp>
#include <array>

namespace vanetza
{

/**
 * A cohesive packet is stored in contiguous memory
 */
class CohesivePacket
{
public:
    typedef boost::iterator_range<ByteBuffer::iterator> buffer_range;
    typedef boost::iterator_range<ByteBuffer::const_iterator> buffer_const_range;

    /**
     * Create packet from buffer and assign all bytes to given layer
     * \param buffer copy data from this buffer
     * \param layer all bytes belong to this layer (at least at first)
     */
    CohesivePacket(const ByteBuffer& buffer, OsiLayer layer);
    CohesivePacket(ByteBuffer&& buffer, OsiLayer layer);

    CohesivePacket(const CohesivePacket&);
    CohesivePacket& operator=(const CohesivePacket&);

    CohesivePacket(CohesivePacket&&) = default;
    CohesivePacket& operator=(CohesivePacket&&) = default;

    /**
     * Access a certain sub-range of packet data belonging to a specific layer
     * \param layer requested layer data
     * \return buffer range with data, might be empty
     */
    buffer_const_range operator[](OsiLayer layer) const;
    buffer_range operator[](OsiLayer layer);

    /**
     * Set boundary of layer data.
     * Data beyond boundary belongs to next upper layer afterwards.
     * \param layer set boundary of this layer
     * \param bytes length of layer
     * \note Never set boundary larger than previous layer length!
     */
    void set_boundary(OsiLayer, unsigned bytes);

    /**
     * Trim size of packet, i.e. cut bytes at the end if too long.
     * \param from start counting with this layer
     * \param bytes target length in bytes
     */
    void trim(OsiLayer from,  unsigned bytes);

    /**
     * Get size of whole packet
     * \return length in bytes
     */
    std::size_t size() const;

    /**
     * Get size of a single layer in packet
     * \param single_layer which layer has to be considered
     * \return length in bytes
     */
    std::size_t size(OsiLayer single_layer) const;

    /**
     * Get size of several layers
     * \param from start counting with this layer
     * \param to stop counting after this layer
     * \return length in bytes
     */
    std::size_t size(OsiLayer from, OsiLayer to) const;

    /**
     * Non-mutable access to internal byte buffer
     * \return const byte buffer reference
     */
    const ByteBuffer& buffer() const { return m_buffer; }

private:
    void reset_iterators(OsiLayer layer);
    void rebuild_iterators(const CohesivePacket&);
    buffer_const_range get(unsigned idx) const;
    buffer_range get(unsigned idx);

    ByteBuffer m_buffer;
    std::array<ByteBuffer::iterator, osi_layers.size() + 1> m_iterators;
};

} // namespace vanetza

#endif /* COHESIVE_PACKET_HPP_VG2XKSCV */

