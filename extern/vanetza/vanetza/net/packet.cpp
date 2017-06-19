#include <vanetza/net/packet.hpp>

namespace vanetza
{

std::unique_ptr<DownPacket> duplicate(const PacketVariant& packet)
{
    struct duplication_visitor : public boost::static_visitor<>
    {
        void operator()(const CohesivePacket& packet)
        {
            m_duplicate.reset(new ChunkPacket());
            for (auto layer : osi_layers) {
                const auto source = packet[layer];
                auto& dest = m_duplicate->layer(layer);
                dest = ByteBuffer(source.begin(), source.end());
            }
        }

        void operator()(const ChunkPacket& packet)
        {
            m_duplicate = duplicate(packet);
        }

        std::unique_ptr<ChunkPacket> m_duplicate;
    };

    duplication_visitor visitor;
    boost::apply_visitor(visitor, packet);
    return std::move(visitor.m_duplicate);
}

std::unique_ptr<DownPacket> duplicate(const DownPacket& packet)
{
    return std::unique_ptr<DownPacket> { new DownPacket(packet) };
}

} // namespace vanetza
