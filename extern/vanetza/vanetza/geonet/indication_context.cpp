#include <vanetza/geonet/indication_context.hpp>
#include <vanetza/geonet/pdu_conversion.hpp>
#include <vanetza/geonet/secured_pdu.hpp>

namespace vanetza
{
namespace geonet
{

IndicationContextDeserialize::IndicationContextDeserialize(UpPacketPtr packet, CohesivePacket& cohesive, const LinkLayer& ll) :
    IndicationContextBasic(ll),
    m_packet(std::move(packet)), m_cohesive_packet(cohesive),
    m_parser(cohesive[OsiLayer::Network])
{
}

const BasicHeader* IndicationContextDeserialize::parse_basic()
{
    auto bytes = m_parser.parse_basic(pdu().basic());
    return bytes > 0 ? &pdu().basic() : nullptr;
}

const CommonHeader* IndicationContextDeserialize::parse_common()
{
    auto bytes = m_parser.parse_common(pdu().common());
    return bytes > 0 ? &pdu().common() : nullptr;
}

const IndicationContext::SecuredMessage* IndicationContextDeserialize::parse_secured()
{
    IndicationContext::SecuredMessage tmp;
    auto bytes = m_parser.parse_secured(tmp);
    if (bytes > 0) {
        pdu().secured(std::move(tmp));
        return pdu().secured();
    } else {
        return nullptr;
    }
}

boost::optional<HeaderConstRefVariant> IndicationContextDeserialize::parse_extended(HeaderType ht)
{
    auto bytes = m_parser.parse_extended(pdu().extended_variant(), ht);
    return boost::optional<HeaderConstRefVariant>(bytes > 0, pdu().extended_variant());
}

IndicationContext::UpPacketPtr IndicationContextDeserialize::finish()
{
    m_cohesive_packet.set_boundary(OsiLayer::Network, m_parser.parsed_bytes());
    m_cohesive_packet.trim(OsiLayer::Transport, pdu().common().payload);
    return std::move(m_packet);
}


IndicationContextCast::IndicationContextCast(UpPacketPtr packet, ChunkPacket& chunk, const LinkLayer& ll) :
    IndicationContextBasic(ll), m_packet(std::move(packet))
{
    Pdu* casted_pdu = pdu_cast(chunk.layer(OsiLayer::Network));
    if (casted_pdu) {
        pdu() = *casted_pdu;
    } else {
        throw std::runtime_error("Casting to Pdu failed");
    }
}

const BasicHeader* IndicationContextCast::parse_basic()
{
    return &pdu().basic();
}

const CommonHeader* IndicationContextCast::parse_common()
{
    return &pdu().common();
}

const IndicationContext::SecuredMessage* IndicationContextCast::parse_secured()
{
    return pdu().secured();
}

boost::optional<HeaderConstRefVariant> IndicationContextCast::parse_extended(HeaderType)
{
    return boost::optional<HeaderConstRefVariant> { pdu().extended_variant() };
}

IndicationContext::UpPacketPtr IndicationContextCast::finish()
{
    // payload should be already in place (if any)
    return std::move(m_packet);
}


IndicationContextSecuredDeserialize::IndicationContextSecuredDeserialize(IndicationContextBasic& parent, CohesivePacket& payload) :
    IndicationContextSecured(parent),
    m_packet(payload),
    m_parser(payload[OsiLayer::Network])
{
}

const CommonHeader* IndicationContextSecuredDeserialize::parse_common()
{
    auto bytes = m_parser.parse_common(pdu().common());
    return bytes > 0 ? &pdu().common() : nullptr;
}

boost::optional<HeaderConstRefVariant> IndicationContextSecuredDeserialize::parse_extended(HeaderType ht)
{
    auto bytes = m_parser.parse_extended(pdu().extended_variant(), ht);
    return boost::optional<HeaderConstRefVariant>(bytes > 0, pdu().extended_variant());
}

IndicationContext::UpPacketPtr IndicationContextSecuredDeserialize::finish()
{
    m_packet.set_boundary(OsiLayer::Network, m_parser.parsed_bytes());
    auto packet = m_parent.finish();
    (*packet) = m_packet;
    return packet;
}


IndicationContextSecuredCast::IndicationContextSecuredCast(IndicationContextBasic& parent, ChunkPacket& packet) :
    IndicationContextSecured(parent),
    m_packet(parent.finish())
{
    SecuredPdu* secured_pdu = secured_pdu_cast(packet.layer(OsiLayer::Network));
    if (secured_pdu) {
        pdu().common() = secured_pdu->common;
        pdu().extended_variant() = secured_pdu->extended;
    } else {
        throw std::runtime_error("Casting to SecuredPdu failed");
    }

    struct parent_packet_visitor : public boost::static_visitor<>
    {
        parent_packet_visitor(ChunkPacket& _secured_payload) : secured_payload(_secured_payload) {}

        void operator()(ChunkPacket& packet)
        {
            packet.merge(secured_payload, OsiLayer::Transport, max_osi_layer());
        }

        void operator()(CohesivePacket& packet)
        {
            // CohesivePacket and casting PDUs will probably never happen...
            ByteBuffer buffer(secured_payload.size());
            for (auto layer : osi_layer_range(OsiLayer::Transport, max_osi_layer())) {
                ByteBuffer layer_buffer;
                secured_payload.layer(layer).convert(layer_buffer);
                buffer.insert(buffer.end(), layer_buffer.begin(), layer_buffer.end());
            }
            packet = CohesivePacket(std::move(buffer), OsiLayer::Transport);
        }

        ChunkPacket& secured_payload;
    };

    parent_packet_visitor visitor(packet);
    boost::apply_visitor(visitor, *m_packet);
}

const CommonHeader* IndicationContextSecuredCast::parse_common()
{
    return &pdu().common();
}

boost::optional<HeaderConstRefVariant> IndicationContextSecuredCast::parse_extended(HeaderType)
{
    return boost::optional<HeaderConstRefVariant> { pdu().extended_variant() };
}

IndicationContext::UpPacketPtr IndicationContextSecuredCast::finish()
{
    return std::move(m_packet);
}

} // namespace geonet
} // namespace vanetza
