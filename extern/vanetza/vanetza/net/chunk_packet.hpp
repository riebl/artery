#ifndef CHUNK_PACKET_HPP_AT4GYSLD
#define CHUNK_PACKET_HPP_AT4GYSLD

#include <vanetza/common/byte_buffer_convertible.hpp>
#include <vanetza/net/osi_layer.hpp>
#include <cstddef>
#include <map>

namespace vanetza
{

/**
 * A packet consisting of several memory chunks
 */
class ChunkPacket
{
public:
    ChunkPacket();

    // copy semantics
    ChunkPacket(const ChunkPacket&);
    ChunkPacket& operator=(const ChunkPacket&);

    // move semantics
    ChunkPacket(ChunkPacket&&) = default;
    ChunkPacket& operator=(ChunkPacket&&) = default;

    /**
     * Access ByteBufferConvertible of specific layer
     * \param layer Access this layer's data
     * \return ByteBufferConvertible, might be empty
     */
    ByteBufferConvertible& layer(OsiLayer);
    const ByteBufferConvertible& layer(OsiLayer) const;

    inline ByteBufferConvertible& operator[](OsiLayer ol)
    {
        return layer(ol);
    }

    inline const ByteBufferConvertible& operator[](OsiLayer ol) const
    {
        return layer(ol);
    }

    /**
     * Get size of whole payload
     * \return payload size in bytes
     */
    std::size_t size() const;

    /**
     * Get size of payload from specified layer range
     * \param from start counting including this layer
     * \param to stop counting including this layer
     * \return payload size in bytes
     */
    std::size_t size(OsiLayer from, OsiLayer to) const;

    /**
     * Extract a range of layers from this packet to a new one
     * \param from start at this layer (inclusive)
     * \param to stop at this layer (inclusive)
     * \return new packet containing layers of specified range
     */
    ChunkPacket extract(OsiLayer from, OsiLayer to);

    /**
     * Merge layers from another packet
     * \param packet source packet (layers will be moved from there)
     * \param from start at this layer (inclusive)
     * \param to stop at this layer (inclusive)
     * \return reference to this packet
     */
    ChunkPacket& merge(ChunkPacket&, OsiLayer from, OsiLayer to);

private:
    typedef std::map<OsiLayer, ByteBufferConvertible> map_type;
    map_type m_layers;
};

} // namespace vanetza

#endif /* CHUNK_PACKET_HPP_AT4GYSLD */

