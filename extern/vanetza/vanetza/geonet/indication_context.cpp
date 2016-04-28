#include <vanetza/geonet/indication_context.hpp>
#include <vanetza/geonet/pdu_conversion.hpp>
#include <vanetza/geonet/secured_pdu.hpp>
#include <vanetza/security/exception.hpp>

namespace vanetza
{
namespace geonet
{

namespace detail
{

IndicationContextDeserializer::IndicationContextDeserializer(const CohesivePacket& packet) :
    m_byte_buffer_source(packet[OsiLayer::Network]),
    m_stream(m_byte_buffer_source),
    m_archive(m_stream, boost::archive::no_header),
    m_read_bytes(0)
{
}

std::size_t IndicationContextDeserializer::parse_basic(BasicHeader& basic)
{
    std::size_t bytes = 0;
    try {
        deserialize(basic, m_archive);
        bytes = BasicHeader::length_bytes;
    } catch (boost::archive::archive_exception&) {
    }

    m_read_bytes += bytes;
    return bytes;
}

std::size_t IndicationContextDeserializer::parse_common(CommonHeader& common)
{
    std::size_t bytes = 0;
    try {
        deserialize(common, m_archive);
        bytes = CommonHeader::length_bytes;
    } catch (boost::archive::archive_exception&) {
    }

    m_read_bytes += bytes;
    return bytes;
}

std::size_t IndicationContextDeserializer::parse_secured(IndicationContext::SecuredMessage& secured)
{
    std::size_t bytes = 0;
    try {
        bytes = deserialize(m_archive, secured);
    } catch (boost::archive::archive_exception&) {
    } catch (security::deserialization_error&) {
    }

    m_read_bytes += bytes;
    return bytes;
}

std::size_t IndicationContextDeserializer::parse_extended(HeaderVariant& extended, HeaderType ht)
{
    std::size_t bytes = 0;

    try {
        switch (ht) {
            case HeaderType::TSB_SINGLE_HOP: {
                    ShbHeader shb;
                    deserialize(shb, m_archive);
                    extended = std::move(shb);
                    bytes = ShbHeader::length_bytes;
                }
                break;
            case HeaderType::GEOBROADCAST_CIRCLE:
            case HeaderType::GEOBROADCAST_RECT:
            case HeaderType::GEOBROADCAST_ELIP: {
                    GeoBroadcastHeader gbc;
                    deserialize(gbc, m_archive);
                    extended = std::move(gbc);
                    bytes = GeoBroadcastHeader::length_bytes;
                }
                break;
            case HeaderType::BEACON: {
                    BeaconHeader beacon;
                    deserialize(beacon, m_archive);
                    extended = std::move(beacon);
                    bytes = BeaconHeader::length_bytes;
                }
                break;
            case HeaderType::ANY:
            case HeaderType::GEOUNICAST:
            case HeaderType::GEOANYCAST_CIRCLE:
            case HeaderType::GEOANYCAST_RECT:
            case HeaderType::GEOANYCAST_ELIP:
            case HeaderType::TSB_MULTI_HOP:
            case HeaderType::LS_REQUEST:
            case HeaderType::LS_REPLY:
                // unimplemented types
                break;
            default:
                // invalid types
                break;
        }
    } catch (boost::archive::archive_exception&) {
    }

    m_read_bytes += bytes;
    return bytes;
}

std::size_t IndicationContextDeserializer::parsed_bytes() const
{
    return m_read_bytes;
}

} // namespace detail


IndicationContextDeserialize::IndicationContextDeserialize(UpPacketPtr packet, CohesivePacket& cohesive, const LinkLayer& ll) :
    detail::IndicationContextParent(ll),
    detail::IndicationContextDeserializer(cohesive),
    m_packet(std::move(packet)), m_cohesive_packet(cohesive)
{
}

BasicHeader* IndicationContextDeserialize::parse_basic()
{
    auto bytes = detail::IndicationContextDeserializer::parse_basic(pdu().basic());
    return bytes > 0 ? &pdu().basic() : nullptr;
}

CommonHeader* IndicationContextDeserialize::parse_common()
{
    auto bytes = detail::IndicationContextDeserializer::parse_common(pdu().common());
    return bytes > 0 ? &pdu().common() : nullptr;
}

IndicationContext::SecuredMessage* IndicationContextDeserialize::parse_secured()
{
    IndicationContext::SecuredMessage tmp;
    auto bytes = detail::IndicationContextDeserializer::parse_secured(tmp);
    if (bytes > 0) {
        pdu().secured(std::move(tmp));
        return pdu().secured();
    } else {
        return nullptr;
    }
}

boost::optional<HeaderConstRefVariant> IndicationContextDeserialize::parse_extended(HeaderType ht)
{
    auto bytes = detail::IndicationContextDeserializer::parse_extended(pdu().extended_variant(), ht);
    return boost::optional<HeaderConstRefVariant>(bytes > 0, pdu().extended_variant());
}

IndicationContext::UpPacketPtr IndicationContextDeserialize::finish()
{
    m_cohesive_packet.set_boundary(OsiLayer::Network, parsed_bytes());
    return std::move(m_packet);
}

std::size_t IndicationContextDeserialize::payload_length() const
{
    return m_cohesive_packet.size(OsiLayer::Network, max_osi_layer()) - parsed_bytes();
}


IndicationContextCast::IndicationContextCast(UpPacketPtr packet, ChunkPacket& chunk, const LinkLayer& ll) :
    detail::IndicationContextParent(ll), m_packet(std::move(packet))
{
    using convertible_pdu_t = convertible::byte_buffer_impl<std::unique_ptr<Pdu>>;
    auto convertible = chunk.layer(OsiLayer::Network).ptr();
    auto pdu_rx = dynamic_cast<convertible_pdu_t*>(convertible);
    if (pdu_rx) {
        pdu() = *pdu_rx->m_pdu;
    } else {
        throw std::runtime_error("Casting to Pdu failed");
    }
}

BasicHeader* IndicationContextCast::parse_basic()
{
    return &pdu().basic();
}

CommonHeader* IndicationContextCast::parse_common()
{
    return &pdu().common();
}

IndicationContext::SecuredMessage* IndicationContextCast::parse_secured()
{
    return pdu().secured();
}

boost::optional<HeaderConstRefVariant> IndicationContextCast::parse_extended(HeaderType)
{
    boost::optional<HeaderConstRefVariant> extended;
    HeaderConstRefVariant variant = pdu().extended_variant();
    extended.emplace(variant);
    return extended;
}

IndicationContext::UpPacketPtr IndicationContextCast::finish()
{
    // payload should be already in place (if any)
    return std::move(m_packet);
}

std::size_t IndicationContextCast::payload_length() const
{
    return m_packet ? size(*m_packet, OsiLayer::Transport, max_osi_layer()) : 0;
}

IndicationContextSecuredDeserialize::IndicationContextSecuredDeserialize(IndicationContext& parent, CohesivePacket& payload) :
    detail::IndicationContextChild(parent),
    detail::IndicationContextDeserializer(payload),
    m_packet(payload)
{
}

CommonHeader* IndicationContextSecuredDeserialize::parse_common()
{
    auto bytes = detail::IndicationContextDeserializer::parse_common(pdu().common());
    return bytes > 0 ? &pdu().common() : nullptr;
}

boost::optional<HeaderConstRefVariant> IndicationContextSecuredDeserialize::parse_extended(HeaderType ht)
{
    auto bytes = detail::IndicationContextDeserializer::parse_extended(pdu().extended_variant(), ht);
    return boost::optional<HeaderConstRefVariant>(bytes > 0, pdu().extended_variant());
}

IndicationContext::UpPacketPtr IndicationContextSecuredDeserialize::finish()
{
    m_packet.set_boundary(OsiLayer::Network, parsed_bytes());
    auto packet = m_parent.finish();
    (*packet) = m_packet;
    return packet;
}

std::size_t IndicationContextSecuredDeserialize::payload_length() const
{
    return m_packet.size(OsiLayer::Network, max_osi_layer()) - parsed_bytes();
}

IndicationContextSecuredCast::IndicationContextSecuredCast(IndicationContext& parent, ChunkPacket& packet) :
    detail::IndicationContextChild(parent),
    m_packet(parent.finish())
{
    using convertible_pdu_t = convertible::byte_buffer_impl<SecuredPdu>;
    auto convertible = packet.layer(OsiLayer::Network).ptr();
    auto pdu_rx = dynamic_cast<convertible_pdu_t*>(convertible);
    if (pdu_rx) {
        pdu().common() = pdu_rx->pdu.common;
        pdu().extended_variant() = pdu_rx->pdu.extended;
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

CommonHeader* IndicationContextSecuredCast::parse_common()
{
    return &pdu().common();
}

boost::optional<HeaderConstRefVariant> IndicationContextSecuredCast::parse_extended(HeaderType)
{
    boost::optional<HeaderConstRefVariant> extended;
    HeaderConstRefVariant variant = pdu().extended_variant();
    extended.emplace(variant);
    return extended;
}

IndicationContext::UpPacketPtr IndicationContextSecuredCast::finish()
{
    return std::move(m_packet);
}

std::size_t IndicationContextSecuredCast::payload_length() const
{
    return m_packet ? size(*m_packet, OsiLayer::Transport, max_osi_layer()) : 0;
}

} // namespace geonet
} // namespace vanetza
