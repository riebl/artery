#include "packet_variant.hpp"
#include <boost/variant.hpp>

namespace vanetza
{

byte_view_range create_byte_view(const ChunkPacket& packet, OsiLayer layer)
{
    return create_byte_view(packet[layer]);
}

byte_view_range create_byte_view(const CohesivePacket& packet, OsiLayer layer)
{
    CohesivePacket::buffer_const_range range = packet[layer];
    return byte_view_range { range.begin(), range.end() };
}

void serialize(OutputArchive& oa, const ChunkPacket& packet)
{
    ByteBuffer buf;
    for (auto layer : osi_layers) {
        buf.clear();
        packet[layer].convert(buf);
        oa.save_binary(buf.data(), buf.size());
    }
}

void serialize(OutputArchive& oa, const CohesivePacket& packet)
{
    oa.save_binary(packet.buffer().data(), packet.buffer().size());
}

} // namespace vanetza

namespace boost
{

using namespace vanetza;

std::size_t size(const PacketVariant& packet, OsiLayer layer)
{
    struct size_visitor : public boost::static_visitor<std::size_t>
    {
        size_visitor(OsiLayer layer) : m_layer(layer) {}

        std::size_t operator()(const CohesivePacket& packet)
        {
            return size(packet, m_layer);
        }

        std::size_t operator()(const ChunkPacket& packet)
        {
            return size(packet, m_layer);
        }

        OsiLayer m_layer;
    };

    size_visitor visitor(layer);
    return boost::apply_visitor(visitor, packet);
}

std::size_t size(const PacketVariant& packet, OsiLayer from, OsiLayer to)
{
    struct size_visitor : public boost::static_visitor<std::size_t>
    {
        size_visitor(OsiLayer from, OsiLayer to) : m_from(from), m_to(to) {}

        std::size_t operator()(const CohesivePacket& packet)
        {
            return size(packet, m_from, m_to);
        }

        std::size_t operator()(const ChunkPacket& packet)
        {
            return size(packet, m_from, m_to);
        }

        OsiLayer m_from;
        OsiLayer m_to;
    };

    size_visitor visitor(from, to);
    return boost::apply_visitor(visitor, packet);
}

byte_view_range create_byte_view(const PacketVariant& packet, OsiLayer layer)
{
    struct payload_visitor : public boost::static_visitor<byte_view_range>
    {
        payload_visitor(OsiLayer layer) : m_layer(layer) {}

        byte_view_range operator()(const CohesivePacket& packet)
        {
            return create_byte_view(packet, m_layer);
        }

        byte_view_range operator()(const ChunkPacket& packet)
        {
            return create_byte_view(packet, m_layer);
        }

        OsiLayer m_layer;
    };

    payload_visitor visitor(layer);
    return boost::apply_visitor(visitor, packet);
}

void serialize(OutputArchive& ar, const PacketVariant& packet)
{
    struct serialize_visitor : public boost::static_visitor<>
    {
        serialize_visitor(OutputArchive& _oa) : oa(_oa) {}

        void operator()(const ChunkPacket& packet)
        {
            serialize(oa, packet);
        }

        void operator()(const CohesivePacket& packet)
        {
            serialize(oa, packet);
        }

        OutputArchive& oa;
    };

    serialize_visitor visitor(ar);
    boost::apply_visitor(visitor, packet);
}

} // namespace boost
