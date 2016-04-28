#include "port_dispatcher.hpp"
#include "data_indication.hpp"
#include <vanetza/geonet/data_indication.hpp>
#include <vanetza/common/serialization_buffer.hpp>
#include <cassert>

namespace vanetza
{
namespace btp
{

HeaderB parse_btp_b(CohesivePacket& packet)
{
    HeaderB hdr;
    deserialize_from_range(hdr, packet[OsiLayer::Transport]);
    packet.set_boundary(OsiLayer::Transport, btp::HeaderB::length_bytes);
    return hdr;
}

HeaderB parse_btp_b(ChunkPacket& packet)
{
    HeaderB hdr;
    ByteBuffer tmp;
    packet[OsiLayer::Transport].convert(tmp);
    deserialize_from_buffer(hdr, tmp);
    return hdr;
}

HeaderB parse_btp_b(PacketVariant& packet)
{
    struct parse_btp_visitor : public boost::static_visitor<HeaderB>
    {
        HeaderB operator()(CohesivePacket& packet) {
            return parse_btp_b(packet);
        }

        HeaderB operator()(ChunkPacket& packet) {
            return parse_btp_b(packet);
        }
    };

    parse_btp_visitor visitor;
    return boost::apply_visitor(visitor, packet);
}

boost::optional<DataIndication> parse_btp_header(const geonet::DataIndication& gn_ind, PacketVariant& packet)
{
    boost::optional<DataIndication> indication;

    switch (gn_ind.upper_protocol) {
        case geonet::UpperProtocol::BTP_A:
            // TODO: parse BTP-A packets
            break;
        case geonet::UpperProtocol::BTP_B: {
            HeaderB hdr = parse_btp_b(packet);
            indication = DataIndication(gn_ind, hdr);
            }
            break;
        default:
            // drop non-BTP packet
            break;
    }

    return indication;
}

void PortDispatcher::add_promiscuous_hook(PromiscuousHook* hook)
{
    if (hook != nullptr) {
        m_promiscuous_hooks.push_back(hook);
    }
}

void PortDispatcher::set_non_interactive_handler(
        port_type port,
        IndicationInterface* handler)
{
    m_non_interactive_handlers[port] = handler;
}

void PortDispatcher::indicate(
        const geonet::DataIndication& gn_ind,
        std::unique_ptr<UpPacket> packet)
{
    assert(packet);
    boost::optional<DataIndication> btp_ind = parse_btp_header(gn_ind, *packet);
    IndicationInterface* handler = nullptr;

    if (btp_ind) {
        if (!btp_ind->source_port) {
            handler = m_non_interactive_handlers[btp_ind->destination_port];
        }

        for (PromiscuousHook* hook : m_promiscuous_hooks) {
            hook->tap_packet(*btp_ind, *packet);
        }

        if (nullptr == handler) {
            hook_undispatched(gn_ind, &btp_ind.get());
        } else {
            handler->indicate(*btp_ind, std::move(packet));
        }
    } else {
        hook_undispatched(gn_ind, nullptr);
    }
}

} // namespace btp
} // namespace vanetza

