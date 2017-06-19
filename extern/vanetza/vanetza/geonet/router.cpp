#include <vanetza/btp/data_indication.hpp>
#include <vanetza/btp/data_request.hpp>
#include <vanetza/common/runtime.hpp>
#include <vanetza/dcc/access_control.hpp>
#include <vanetza/dcc/data_request.hpp>
#include <vanetza/dcc/profile.hpp>
#include <vanetza/net/mac_address.hpp>
#include <vanetza/net/osi_layer.hpp>
#include <vanetza/units/frequency.hpp>
#include <vanetza/units/length.hpp>
#include <vanetza/units/time.hpp>
#include <vanetza/geonet/router.hpp>
#include <vanetza/geonet/data_confirm.hpp>
#include <vanetza/geonet/indication_context.hpp>
#include <vanetza/geonet/next_hop.hpp>
#include <vanetza/geonet/pdu_conversion.hpp>
#include <vanetza/geonet/repetition_dispatcher.hpp>
#include <vanetza/geonet/transport_interface.hpp>
#include <vanetza/geonet/extended_pdu.hpp>
#include <vanetza/geonet/secured_pdu.hpp>
#include <boost/units/cmath.hpp>
#include <functional>
#include <stdexcept>
#include <tuple>
#include <type_traits>

namespace vanetza
{
namespace geonet
{

// all referenced sections belong to TS 102 636-4 (v1.2.3)

template<typename PDU>
class PacketBufferData : public packet_buffer::Data
{
public:
    PacketBufferData(Router& router, std::unique_ptr<PDU> pdu, std::unique_ptr<DownPacket> payload) :
        m_router(router), m_pdu(std::move(pdu)), m_payload(std::move(payload))
    {
    }

    std::size_t length() const override
    {
        assert(m_pdu && m_payload);
        return get_length(*m_pdu) +
                m_payload->size(OsiLayer::Transport, max_osi_layer());
    }

    Lifetime& lifetime() override
    {
        assert(m_pdu);
        return m_pdu->basic().lifetime;
    }

protected:
    Router& m_router;
    std::unique_ptr<PDU> m_pdu;
    std::unique_ptr<DownPacket> m_payload;
};

class BroadcastBufferData : public PacketBufferData<Pdu>
{
public:
    using PacketBufferData<Pdu>::PacketBufferData;

    NextHop flush() override
    {
        NextHop next_hop;
        next_hop.mac(cBroadcastMacAddress);
        next_hop.data(std::move(m_pdu), std::move(m_payload));
        next_hop.state(NextHop::State::VALID);
        return next_hop;
    }
};

/**
 * DCC request interface used when no user supplied interface is available.
 */
class DummyDccRequestInterface : public dcc::RequestInterface
{
public:
    void request(const dcc::DataRequest&, std::unique_ptr<ChunkPacket>) override {}
    static dcc::RequestInterface& get()
    {
        static DummyDccRequestInterface ifc;
        return ifc;
    }

private:
    DummyDccRequestInterface() {};
};

using units::clock_cast;

template<typename PDU>
auto create_forwarding_duplicate(const PDU& pdu, const UpPacket& packet) ->
std::tuple<
    std::unique_ptr<typename std::remove_pointer<decltype(pdu.clone())>::type>,
    std::unique_ptr<DownPacket>
>
{
    using PduCloneType = typename std::remove_pointer<decltype(pdu.clone())>::type;
    std::unique_ptr<PduCloneType> pdu_dup { pdu.clone() };
    std::unique_ptr<DownPacket> packet_dup;
    if (pdu.secured()) {
        packet_dup.reset(new DownPacket());
    } else {
        packet_dup = duplicate(packet);
    }
    return std::make_tuple(std::move(pdu_dup), std::move(packet_dup));
}

const uint16be_t ether_type = host_cast<uint16_t>(0x8947);

Router::Router(Runtime& rt, const MIB& mib) :
    m_mib(mib),
    m_runtime(rt),
    m_request_interface(&DummyDccRequestInterface::get()),
    m_security_entity(nullptr),
    m_location_table(mib, m_runtime),
    m_bc_forward_buffer(mib.itsGnBcForwardingPacketBufferSize * 1024),
    m_uc_forward_buffer(mib.itsGnUcForwardingPacketBufferSize * 1024),
    m_cbf_buffer(m_runtime,
            std::bind(&Router::on_cbf_timer_expiration, this, std::placeholders::_1),
            mib.itsGnCbfPacketBufferSize * 1024),
    m_repeater(m_runtime,
            std::bind(&Router::dispatch_repetition, this, std::placeholders::_1, std::placeholders::_2))
{
    // send BEACON immediately after start-up at next runtime trigger invocation
    reset_beacon_timer(Clock::duration::zero());
}

Router::~Router()
{
}

void Router::update(const LongPositionVector& lpv)
{
    // Update LPV except for GN address
    Address gn_addr = m_local_position_vector.gn_addr;
    m_local_position_vector = lpv;
    m_local_position_vector.gn_addr = gn_addr;
}

void Router::set_transport_handler(UpperProtocol proto, TransportInterface* ifc)
{
    m_transport_ifcs[proto] = ifc;
}

void Router::set_security_entity(security::SecurityEntity* entity)
{
    m_security_entity = entity;
}

void Router::set_access_interface(dcc::RequestInterface* ifc)
{
    m_request_interface = (ifc == nullptr ? &DummyDccRequestInterface::get() : ifc);
    assert(m_request_interface != nullptr);
}

void Router::set_address(const Address& addr)
{
    m_local_position_vector.gn_addr = addr;
}

void Router::set_random_seed(std::uint_fast32_t seed)
{
    m_random_gen.seed(seed);
}

DataConfirm Router::request(const ShbDataRequest& request, DownPacketPtr payload)
{
    DataConfirm result;
    result ^= validate_data_request(request, m_mib);
    result ^= validate_payload(payload, m_mib);

    if (result.accepted()) {
        auto pdu = create_shb_pdu(request);
        pdu->common().payload = payload->size();

        // forward buffering
        if (request.traffic_class.store_carry_forward() && !m_location_table.has_neighbours()) {
            if (m_mib.itsGnSecurity) {
                payload = encap_packet(request.security_profile, *pdu, std::move(payload));
            }
            std::unique_ptr<BroadcastBufferData> data {
                new BroadcastBufferData(*this, std::move(pdu), std::move(payload))
            };
            m_bc_forward_buffer.push(std::move(data), m_runtime.now());
        } else {
            if (request.repetition) {
                // plaintext payload needs to get passed
                m_repeater.add(request, *payload);
            }
            if (m_mib.itsGnSecurity) {
                payload = encap_packet(request.security_profile, *pdu, std::move(payload));
            }

            execute_media_procedures(request.communication_profile);
            pass_down(cBroadcastMacAddress, std::move(pdu), std::move(payload));
            reset_beacon_timer();
        }
    }

    return result;
}

DataConfirm Router::request(const GbcDataRequest& request, DownPacketPtr payload)
{
    DataConfirm result;
    result ^= validate_data_request(request, m_mib);
    result ^= validate_payload(payload, m_mib);

    if (result.accepted()) {
        auto pdu = create_gbc_pdu(request);
        pdu->common().payload = payload->size();


        // Set up packet repetition (plaintext payload)
        if (request.repetition) {
            assert(payload);
            m_repeater.add(request, *payload);
        }

        // Security
        if (m_mib.itsGnSecurity) {
            assert(pdu->basic().next_header == NextHeaderBasic::SECURED);
            payload = encap_packet(request.security_profile, *pdu, std::move(payload));
        }

        // Forwarding
        NextHop first_hop;
        switch (m_mib.itsGnGeoBroadcastForwardingAlgorithm) {
            case BroadcastForwarding::UNSPECIFIED:
                // do simple forwarding
            case BroadcastForwarding::SIMPLE:
                throw std::runtime_error("simple BC forwarding not implemented");
                break;
            case BroadcastForwarding::CBF:
                first_hop = first_hop_contention_based_forwarding(
                        request.traffic_class.store_carry_forward(),
                        std::move(pdu), std::move(payload));
                break;
            case BroadcastForwarding::ADVANCED:
                first_hop = first_hop_gbc_advanced(request.traffic_class.store_carry_forward(),
                        std::move(pdu), std::move(payload));
                break;
            default:
                throw std::runtime_error("unhandled BC forwarding algorithm");
                break;
        };

        if (first_hop.valid()) {
            execute_media_procedures(request.communication_profile);
            std::unique_ptr<Pdu> pdu;
            std::unique_ptr<DownPacket> payload;
            std::tie(pdu, payload) = first_hop.data();
            pass_down(first_hop.mac(), std::move(pdu), std::move(payload));
        } else {
            result ^= DataConfirm::ResultCode::REJECTED_UNSPECIFIED;
        }
    }

    return result;
}

DataConfirm Router::request(const GacDataRequest&, DownPacketPtr)
{
    return DataConfirm(DataConfirm::ResultCode::REJECTED_UNSPECIFIED);
}

DataConfirm Router::request(const GucDataRequest&, DownPacketPtr)
{
    return DataConfirm(DataConfirm::ResultCode::REJECTED_UNSPECIFIED);
}

DataConfirm Router::request(const TsbDataRequest&, DownPacketPtr)
{
    return DataConfirm(DataConfirm::ResultCode::REJECTED_UNSPECIFIED);
}

void Router::indicate(UpPacketPtr packet, const MacAddress& sender, const MacAddress& destination)
{
    assert(packet);

    struct indication_visitor : public boost::static_visitor<>
    {
        indication_visitor(Router& router, const IndicationContext::LinkLayer& link_layer, UpPacketPtr packet) :
            m_router(router), m_link_layer(link_layer), m_packet(std::move(packet))
        {
        }

        void operator()(CohesivePacket& packet)
        {
            IndicationContextDeserialize ctx(std::move(m_packet), packet, m_link_layer);
            m_router.indicate_basic(ctx);
        }

        void operator()(ChunkPacket& packet)
        {
            IndicationContextCast ctx(std::move(m_packet), packet, m_link_layer);
            m_router.indicate_basic(ctx);
        }

        Router& m_router;
        const IndicationContext::LinkLayer& m_link_layer;
        UpPacketPtr m_packet;
    };

    IndicationContext::LinkLayer link_layer;
    link_layer.sender = sender;
    link_layer.destination = destination;
    UpPacket* packet_tmp = packet.get();
    indication_visitor visitor(*this, link_layer, std::move(packet));
    boost::apply_visitor(visitor, *packet_tmp);
}

void Router::indicate_basic(IndicationContext& ctx)
{
    BasicHeader* basic = ctx.parse_basic();
    if (!basic) {
        packet_dropped(PacketDropReason::PARSE_BASIC_HEADER);
    } else if (basic->version.raw() != m_mib.itsGnProtocolVersion) {
        packet_dropped(PacketDropReason::ITS_PROTOCOL_VERSION);
    } else if (basic->next_header == NextHeaderBasic::SECURED) {
        indicate_secured(ctx, *basic);
    } else if (basic->next_header == NextHeaderBasic::COMMON) {
        indicate_common(ctx, *basic);
    }
}

void Router::indicate_common(IndicationContext& ctx, const BasicHeader& basic)
{
    CommonHeader* common = ctx.parse_common();
    if (!common) {
        packet_dropped(PacketDropReason::PARSE_COMMON_HEADER);
    } else if (common->maximum_hop_limit < basic.hop_limit) {
        packet_dropped(PacketDropReason::HOP_LIMIT);
    } else {
        // clean up location table at packet indication (nothing else creates entries)
        m_location_table.drop_expired();
        flush_broadcast_forwarding_buffer();
        indicate_extended(ctx, *common);
    }
}

void Router::indicate_secured(IndicationContext& ctx, const BasicHeader& basic)
{
    struct secured_payload_visitor : public boost::static_visitor<>
    {
        secured_payload_visitor(Router& router, IndicationContext& ctx, const BasicHeader& basic) :
            m_router(router), m_parent_ctx(ctx), m_basic(basic)
        {
        }

        void operator()(ChunkPacket& packet)
        {
            IndicationContextSecuredCast ctx(m_parent_ctx, packet);
            m_router.indicate_common(ctx, m_basic);
        }

        void operator()(CohesivePacket& packet)
        {
            IndicationContextSecuredDeserialize ctx(m_parent_ctx, packet);
            m_router.indicate_common(ctx, m_basic);
        }

        Router& m_router;
        IndicationContext& m_parent_ctx;
        const BasicHeader& m_basic;
    };

    auto secured_message = ctx.parse_secured();
    if (!secured_message) {
        packet_dropped(PacketDropReason::PARSE_SECURED_HEADER);
    } else if (m_security_entity) {
        // Decap packet
        using namespace vanetza::security;
        DecapConfirm decap_confirm = m_security_entity->decapsulate_packet(DecapRequest(*secured_message));
        secured_payload_visitor visitor(*this, ctx, basic);

        // check whether the received packet is valid
        if (DecapReport::Success == decap_confirm.report) {
            boost::apply_visitor(visitor, decap_confirm.plaintext_payload);
        } else if (SecurityDecapHandling::NON_STRICT == m_mib.itsGnSnDecapResultHandling) {
            // according to ETSI EN 302 636-4-1 v1.2.1 section 9.3.3 Note 2
            // handle the packet anyway, when itsGnDecapResultHandling is set to NON-STRICT (1)
            switch (decap_confirm.report) {
                case DecapReport::False_Signature:
                case DecapReport::Invalid_Certificate:
                case DecapReport::Revoked_Certificate:
                case DecapReport::Inconsistant_Chain:
                case DecapReport::Invalid_Timestamp:
                case DecapReport::Invalid_Mobility_Data:
                case DecapReport::Unsigned_Message:
                case DecapReport::Signer_Certificate_Not_Found:
                case DecapReport::Unsupported_Signer_Identifier_Type:
                case DecapReport::Unencrypted_Message:
                    // ok, continue
                    boost::apply_visitor(visitor, decap_confirm.plaintext_payload);
                    break;
                case DecapReport::Duplicate_Message:
                case DecapReport::Incompatible_Protocol:
                case DecapReport::Decryption_Error:
                default:
                    packet_dropped(PacketDropReason::DECAP_UNSUCCESSFUL_NON_STRICT);
                    break;
            }
        } else {
            // discard packet
            packet_dropped(PacketDropReason::DECAP_UNSUCCESSFUL_STRICT);
        }
    } else {
        packet_dropped(PacketDropReason::SECURITY_ENTITY_MISSING);
    }
}

void Router::indicate_extended(IndicationContext& ctx, const CommonHeader& common)
{
    struct extended_header_visitor : public boost::static_visitor<>
    {
        extended_header_visitor(Router* router, IndicationContext& ctx, UpPacketPtr packet) :
            m_router(router), m_context(ctx), m_packet(std::move(packet))
        {
        }

        void operator()(const ShbHeader& shb)
        {
            auto& pdu = m_context.pdu();
            ExtendedPduConstRefs<ShbHeader> shb_pdu(pdu.basic(), pdu.common(), shb, pdu.secured());
            m_router->process_extended(shb_pdu, std::move(m_packet));
        }

        void operator()(const GeoBroadcastHeader& gbc)
        {
            auto& pdu = m_context.pdu();
            ExtendedPduConstRefs<GeoBroadcastHeader> gbc_pdu(pdu.basic(), pdu.common(), gbc, pdu.secured());
            const IndicationContext::LinkLayer& ll = m_context.link_layer();
            m_router->process_extended(gbc_pdu, std::move(m_packet), ll.sender, ll.destination);
        }

        void operator()(const BeaconHeader& beacon)
        {
            auto& pdu = m_context.pdu();
            ExtendedPduConstRefs<BeaconHeader> beacon_pdu(pdu.basic(), pdu.common(), beacon, pdu.secured());
            m_router->process_extended(beacon_pdu, std::move(m_packet));
        }

        Router* m_router;
        IndicationContext& m_context;
        UpPacketPtr m_packet;
    };

    auto extended = ctx.parse_extended(common.header_type);
    UpPacketPtr packet = ctx.finish();

    if (!extended) {
        packet_dropped(PacketDropReason::PARSE_EXTENDED_HEADER);
    } else if (common.payload != size(*packet, OsiLayer::Transport, max_osi_layer())) {
        packet_dropped(PacketDropReason::PAYLOAD_SIZE);
    } else {
        extended_header_visitor visitor(this, ctx, std::move(packet));
        boost::apply_visitor(visitor, *extended);
    }
}

void Router::execute_media_procedures(CommunicationProfile com_profile)
{
    switch (com_profile) {
        case CommunicationProfile::ITS_G5:
            execute_itsg5_procedures();
            break;
        case CommunicationProfile::UNSPECIFIED:
            // do nothing
            break;
        default:
            throw std::runtime_error("Unhandled communication profile");
            break;
    }
}

void Router::execute_itsg5_procedures()
{
    // TODO: implement ITS_G5A procedures, see EN 302636-4-2
}

void Router::pass_down(const dcc::DataRequest& request, PduPtr pdu, DownPacketPtr payload)
{
    assert(pdu);
    assert(payload);
    if (pdu->secured()) {
        if (pdu->basic().next_header != NextHeaderBasic::SECURED) {
            throw std::runtime_error("PDU with secured message but SECURED not set in basic header");
        }
        if (payload->size(OsiLayer::Transport, max_osi_layer()) > 0) {
            throw std::runtime_error("PDU with secured message and illegal upper layer payload");
        }
    } else {
        if (pdu->basic().next_header == NextHeaderBasic::SECURED) {
            throw std::runtime_error("PDU without secured message but SECURED set in basic header");
        }
    }

    (*payload)[OsiLayer::Network] = ByteBufferConvertible(std::move(pdu));
    assert(m_request_interface);
    m_request_interface->request(request, std::move(payload));
}

void Router::pass_down(const MacAddress& addr, PduPtr pdu, DownPacketPtr payload)
{
    assert(pdu);

    dcc::DataRequest request;
    request.destination = addr;
    request.source = m_local_position_vector.gn_addr.mid();
    request.dcc_profile = map_tc_onto_profile(pdu->common().traffic_class);
    request.ether_type = geonet::ether_type;
    request.lifetime = std::chrono::seconds(pdu->basic().lifetime.decode() / units::si::seconds);

    pass_down(request, std::move(pdu), std::move(payload));
}

void Router::pass_up(DataIndication& ind, UpPacketPtr packet)
{
    TransportInterface* transport = m_transport_ifcs[ind.upper_protocol];
    if (transport != nullptr) {
        transport->indicate(ind, std::move(packet));
    }
}

void Router::on_beacon_timer_expired()
{
    // BEACONs originate in GeoNet layer, therefore no upper layer payload
    DownPacketPtr payload { new DownPacket() };
    auto pdu = create_beacon_pdu();

    if (m_mib.itsGnSecurity) {
        pdu->basic().next_header = NextHeaderBasic::SECURED;
        payload = encap_packet(security::Profile::Generic, *pdu, std::move(payload));
    } else {
        pdu->basic().next_header = NextHeaderBasic::COMMON;
    }

    execute_media_procedures(m_mib.itsGnIfType);
    pass_down(cBroadcastMacAddress, std::move(pdu), std::move(payload));
    reset_beacon_timer();
}

void Router::reset_beacon_timer()
{
    using duration_t = decltype(m_mib.itsGnBeaconServiceRetransmitTimer);
    using real_t = duration_t::value_type;

    std::uniform_real_distribution<real_t> dist_jitter(0.0, 1.0);
    const auto jitter = dist_jitter(m_random_gen);
    const duration_t next_beacon = m_mib.itsGnBeaconServiceRetransmitTimer +
        jitter * m_mib.itsGnBeaconServiceMaxJitter;
    reset_beacon_timer(clock_cast(next_beacon));
}

void Router::reset_beacon_timer(Clock::duration next_beacon)
{
    static const std::string beacon_timer_name = "geonet.beacon.timer";
    m_runtime.cancel(beacon_timer_name);
    m_runtime.schedule(next_beacon, [this](Clock::time_point) {
        on_beacon_timer_expired();
    }, beacon_timer_name);
}

void Router::dispatch_repetition(const DataRequestVariant& request, std::unique_ptr<DownPacket> payload)
{
    RepetitionDispatcher dispatcher(*this, std::move(payload));
    boost::apply_visitor(dispatcher, request);
}

NextHop Router::first_hop_contention_based_forwarding(
        bool scf,
        std::unique_ptr<GbcPdu> pdu, DownPacketPtr payload)
{
    // TODO: EN 302 636-4-1 v1.2.5 does broadcast in any case
    NextHop nh;
    const Area& destination = pdu->extended().destination(pdu->common().header_type);
    if (inside_or_at_border(destination, m_local_position_vector.position())) {
        nh.mac(cBroadcastMacAddress);
        nh.data(std::move(pdu), std::move(payload));
        nh.state(NextHop::State::VALID);
    } else {
        nh = next_hop_greedy_forwarding(scf, std::move(pdu), std::move(payload));
    }
    return nh;
}

NextHop Router::next_hop_contention_based_forwarding(
        bool scf, const MacAddress& sender,
        std::unique_ptr<GbcPdu> pdu, DownPacketPtr payload)
{
    NextHop nh;
    const GeoBroadcastHeader& gbc = pdu->extended();
    const HeaderType ht = pdu->common().header_type;

    if (m_cbf_buffer.try_drop(gbc.source_position.gn_addr, gbc.sequence_number)) {
        nh.state(NextHop::State::DISCARDED);
    } else {
        const Area destination_area = gbc.destination(ht);
        if (inside_or_at_border(destination_area, m_local_position_vector.position())) {
            CbfPacket packet(std::move(pdu), std::move(payload));
            m_cbf_buffer.enqueue(std::move(packet), clock_cast(timeout_cbf_gbc(sender)));
            nh.state(NextHop::State::BUFFERED);
        } else {
            auto pv_se = m_location_table.get_position(sender);
            if (pv_se && pv_se->position_accuracy_indicator) {
                if (!inside_or_at_border(destination_area, pv_se->position())) {
                    nh = next_hop_greedy_forwarding(scf, std::move(pdu), std::move(payload));
                } else {
                    nh.state(NextHop::State::DISCARDED);
                }
            } else {
                nh.mac(cBroadcastMacAddress);
                nh.data(std::move(pdu), std::move(payload));
                nh.state(NextHop::State::VALID);
            }
        }
    }

    return nh;
}

NextHop Router::first_hop_gbc_advanced(bool scf, std::unique_ptr<GbcPdu> pdu, DownPacketPtr payload)
{
    const Area& destination = pdu->extended().destination(pdu->common().header_type);
    if (inside_or_at_border(destination, m_local_position_vector.position())) {
        units::Duration timeout = m_mib.itsGnGeoBroadcastCbfMaxTime;
        CbfPacket packet(std::unique_ptr<GbcPdu> { pdu->clone() }, duplicate(*payload));
        m_cbf_buffer.enqueue(std::move(packet), clock_cast(timeout));
    }

    return next_hop_greedy_forwarding(scf, std::move(pdu), std::move(payload));
}

NextHop Router::next_hop_gbc_advanced(
        bool scf, const MacAddress& sender, const MacAddress& destination,
        std::unique_ptr<GbcPdu> pdu, DownPacketPtr payload)
{
    NextHop nh;
    const GeoBroadcastHeader& gbc = pdu->extended();
    const HeaderType ht = pdu->common().header_type;
    const Area destination_area = gbc.destination(ht);
    const std::size_t max_counter = m_mib.vanetzaCbfMaxCounter;
    auto cbf = m_cbf_buffer.fetch(gbc.source_position.gn_addr, gbc.sequence_number);

    if (inside_or_at_border(destination_area, m_local_position_vector.position())) {
        if (cbf) {
            if (cbf->counter() >= max_counter) {
                m_cbf_buffer.try_drop(gbc.source_position.gn_addr, gbc.sequence_number);
                nh.state(NextHop::State::DISCARDED);
            } else {
                if (!outside_sectorial_contention_area(cbf->source().mid(), sender)) {
                    m_cbf_buffer.try_drop(gbc.source_position.gn_addr, gbc.sequence_number);
                    nh.state(NextHop::State::DISCARDED);
                } else {
                    ++cbf->counter();
                    m_cbf_buffer.enqueue(std::move(*cbf), clock_cast(timeout_cbf_gbc(sender)));
                    nh.state(NextHop::State::BUFFERED);
                }
            }
        } else {
            units::Duration timeout = 0.0 * units::si::seconds;
            if (destination == m_local_position_vector.gn_addr.mid()) {
                timeout = m_mib.itsGnGeoUnicastCbfMaxTime;
                nh = next_hop_greedy_forwarding(scf,
                        std::unique_ptr<GbcPdu> { pdu->clone() }, duplicate(*payload));
            } else {
                timeout = timeout_cbf_gbc(sender);
                nh.state(NextHop::State::BUFFERED);
            }

            CbfPacket packet(std::move(pdu), std::move(payload));
            m_cbf_buffer.enqueue(std::move(packet), clock_cast(timeout));
            nh.state(NextHop::State::BUFFERED);
        }
    } else {
        auto pv_se = m_location_table.get_position(sender);
        if (pv_se && pv_se->position_accuracy_indicator) {
            if (!inside_or_at_border(destination_area, pv_se->position())) {
                nh = next_hop_greedy_forwarding(scf, std::move(pdu), std::move(payload));
            } else {
                nh.state(NextHop::State::DISCARDED);
            }
        } else {
            nh.mac(cBroadcastMacAddress);
            nh.data(std::move(pdu), std::move(payload));
            nh.state(NextHop::State::VALID);
        }
    }

    return nh;
}

NextHop Router::next_hop_greedy_forwarding(
        bool scf,
        std::unique_ptr<GbcPdu> pdu, DownPacketPtr payload)
{
    NextHop nh;

    GeodeticPosition dest = pdu->extended().position();
    const units::Length own = distance(dest, m_local_position_vector.position());
    units::Length mfr = own;

    for (auto& neighbour : m_location_table.neighbours()) {
        const units::Length dist = distance(dest, neighbour.position_vector.position());
        if (dist < mfr) {
            nh.mac(neighbour.link_layer_address());
            mfr = dist;
        }
    }

    if (mfr < own) {
        nh.data(std::move(pdu), std::move(payload));
        nh.state(NextHop::State::VALID);
    } else {
        if (!m_location_table.has_neighbours() && scf) {
            class GbcGreedyBufferData : public PacketBufferData<GbcPdu>
            {
            public:
                using PacketBufferData<GbcPdu>::PacketBufferData;

                NextHop flush() override
                {
                    return m_router.next_hop_greedy_forwarding(true, std::move(m_pdu), std::move(m_payload));
                }
            };

            std::unique_ptr<GbcGreedyBufferData> data {
                new GbcGreedyBufferData(*this, std::move(pdu), std::move(payload))
            };
            m_uc_forward_buffer.push(std::move(data), m_runtime.now());
            nh.state(NextHop::State::BUFFERED);
        } else {
            nh.mac(cBroadcastMacAddress);
            nh.data(std::move(pdu), std::move(payload));
            nh.state(NextHop::State::VALID);
        }
    }

    return nh;
}

units::Duration Router::timeout_cbf_gbc(units::Length dist) const
{
    // TODO: media-dependent maximum communication range
    const auto dist_max = m_mib.itsGnDefaultMaxCommunicationRange;
    const auto to_cbf_min = m_mib.itsGnGeoBroadcastCbfMinTime;
    const auto to_cbf_max = m_mib.itsGnGeoBroadcastCbfMaxTime;
    auto to_cbf_gbc = to_cbf_min;

    if (dist <= dist_max) {
        to_cbf_gbc = to_cbf_max + (to_cbf_min - to_cbf_max) / dist_max * dist;
    } else {
        to_cbf_gbc = to_cbf_min;
    }

    return to_cbf_gbc;
}

units::Duration Router::timeout_cbf_gbc(const MacAddress& sender) const
{
    units::Duration timeout = 0.0 * units::si::seconds;
    auto pv_se = m_location_table.get_position(sender);
    if (pv_se && pv_se->position_accuracy_indicator) {
        units::Length dist = distance(pv_se->position(), m_local_position_vector.position());
        timeout = timeout_cbf_gbc(dist);
    } else {
        timeout = m_mib.itsGnGeoBroadcastCbfMaxTime;
    }
    return timeout;
}

bool Router::outside_sectorial_contention_area(const MacAddress& sender, const MacAddress& forwarder) const
{
    using units::si::meter;
    auto position_sender = m_location_table.get_position(sender);
    auto position_forwarder = m_location_table.get_position(forwarder);

    // Assumption: if any position is missing, then sectorial area becomes infinite small
    // As a result of this assumption, everything lays outside then
    if (position_sender && position_forwarder) {
        auto dist_r = distance(position_sender->position(), m_local_position_vector.position());
        auto dist_f = distance(position_forwarder->position(), position_sender->position());
        const auto dist_max = m_mib.itsGnDefaultMaxCommunicationRange;

        auto dist_rf = distance(position_forwarder->position(), m_local_position_vector.position());
        auto angle_fsr = 0.0 * units::si::radians;
        if (dist_r > 0.0 * meter && dist_f > 0.0 * meter) {
            auto cos_fsr = (dist_rf * dist_rf - dist_r * dist_r - dist_f * dist_f) /
                (-2.0 * dist_r * dist_f);
            angle_fsr = boost::units::acos(cos_fsr);
        }
        const auto angle_th = m_mib.itsGnBroadcastCBFDefSectorAngle;

        return !(dist_r < dist_f && dist_f < dist_max && angle_fsr < angle_th);
    } else {
        return true;
    }
}

void Router::on_cbf_timer_expiration(CbfPacket::Data&& packet)
{
    dcc::DataRequest request;
    request.destination = cBroadcastMacAddress;
    request.source = m_local_position_vector.gn_addr.mid();
    request.dcc_profile = dcc::Profile::DP3;
    request.ether_type = geonet::ether_type;
    const auto lifetime = packet.pdu->basic().lifetime.decode();
    request.lifetime = std::chrono::seconds(lifetime / units::si::seconds);
    pass_down(request, std::move(packet.pdu), std::move(packet.payload));
}

void Router::process_extended(const ExtendedPduConstRefs<ShbHeader>& pdu, UpPacketPtr packet)
{
    const ShbHeader& shb = pdu.extended();
    const Address& source_addr = shb.source_position.gn_addr;
    const Timestamp& source_time = shb.source_position.timestamp;

    // execute duplicate packet detection (see A.3)
    if (m_location_table.is_duplicate_packet(source_addr, source_time)) {
        // discard packet
        return;
    }

    // execute duplicate address detection (see 9.2.1.5)
    detect_duplicate_address(source_addr);

    // update location table with SO.PV (see C.2)
    m_location_table.update(shb.source_position);
    auto& source_entry = m_location_table.get_entry(source_addr);
    assert(!is_empty(source_entry.position_vector));

    // update SO.PDR in location table (see B.2)
    const std::size_t packet_size = size(*packet, OsiLayer::Network, OsiLayer::Application);
    source_entry.update_pdr(packet_size);

    // set SO LocTE to neighbour
    source_entry.is_neighbour = true;

    // pass packet to transport interface
    DataIndication ind(pdu.basic(), pdu.common());
    ind.source_position = static_cast<ShortPositionVector>(shb.source_position);
    ind.transport_type = TransportType::SHB;
    pass_up(ind, std::move(packet));
}

void Router::process_extended(const ExtendedPduConstRefs<BeaconHeader>& pdu, UpPacketPtr packet)
{
    const BeaconHeader& beacon = pdu.extended();
    const Address& source_addr = beacon.source_position.gn_addr;
    const Timestamp& source_time = beacon.source_position.timestamp;

    // execute duplicate packet detection (see A.3)
    if (m_location_table.is_duplicate_packet(source_addr, source_time)) {
        // discard packet
        return;
    }

    // execute duplicate address detection (see 9.2.1.5)
    detect_duplicate_address(source_addr);

    // update location table with SO.PV (see C.2)
    m_location_table.update(beacon.source_position);
    auto& source_entry = m_location_table.get_entry(source_addr);

    // update SO.PDR in location table (see B.2)
    const std::size_t packet_size = size(*packet, OsiLayer::Network, OsiLayer::Application);
    source_entry.update_pdr(packet_size);

    // set SO LocTE to neighbour
    source_entry.is_neighbour = true;
}

void Router::process_extended(const ExtendedPduConstRefs<GeoBroadcastHeader>& pdu,
        UpPacketPtr packet, const MacAddress& sender, const MacAddress& destination)
{
    // GBC forwarder and receiver operations (section 9.3.11.3 in EN 302 636-4-1 V1.2.1)
    assert(packet);
    const GeoBroadcastHeader& gbc = pdu.extended();
    const Address& source_addr = gbc.source_position.gn_addr;

    // store flag before is_duplicate_packet check (creates entry if not existing)
    const bool remove_neighbour_flag = !m_location_table.has_entry(source_addr);
    if (m_mib.itsGnGeoBroadcastForwardingAlgorithm == BroadcastForwarding::UNSPECIFIED ||
        m_mib.itsGnGeoBroadcastForwardingAlgorithm == BroadcastForwarding::SIMPLE) {
        const Timestamp& source_time = gbc.source_position.timestamp;
        const SequenceNumber& source_sn = gbc.sequence_number;
        if (m_location_table.is_duplicate_packet(source_addr, source_sn, source_time)) {
            return; // discard packet
        }
    }

    detect_duplicate_address(source_addr);

    const std::size_t packet_size = size(*packet, OsiLayer::Network, OsiLayer::Application);
    m_location_table.update(gbc.source_position);
    auto& source_entry = m_location_table.get_entry(source_addr);
    source_entry.update_pdr(packet_size);
    if (remove_neighbour_flag) {
        source_entry.is_neighbour = false;
    }

    auto fwd_dup = create_forwarding_duplicate(pdu, *packet);
    auto& fwd_pdu = std::get<0>(fwd_dup);
    auto& fwd_packet = std::get<1>(fwd_dup);

    const Area dest_area = gbc.destination(pdu.common().header_type);
    if (inside_or_at_border(dest_area, m_local_position_vector.position())) {
        DataIndication ind(pdu.basic(), pdu.common());
        ind.source_position = static_cast<ShortPositionVector>(gbc.source_position);
        ind.transport_type = TransportType::GBC;
        ind.destination = gbc.destination(pdu.common().header_type);
        pass_up(ind, std::move(packet));
    }

    // TODO: flush SO LS packet buffer if LS_pending, reset LS_pending
    flush_unicast_forwarding_buffer();

    if (pdu.basic().hop_limit <= 1) {
        return; // discard packet (step 9a)
    }
    --fwd_pdu->basic().hop_limit; // step 9b
    assert(fwd_pdu->basic().hop_limit + 1 == pdu.basic().hop_limit);

    const bool scf = pdu.common().traffic_class.store_carry_forward();
    NextHop next_hop;
    switch (m_mib.itsGnGeoBroadcastForwardingAlgorithm) {
        case BroadcastForwarding::UNSPECIFIED:
        case BroadcastForwarding::SIMPLE:
            throw std::runtime_error("simple broadcast forwarding unimplemented");
            break;
        case BroadcastForwarding::CBF:
            next_hop = next_hop_contention_based_forwarding(scf, sender, std::move(fwd_pdu), std::move(fwd_packet));
            break;
        case BroadcastForwarding::ADVANCED:
            next_hop = next_hop_gbc_advanced(scf, sender, destination, std::move(fwd_pdu), std::move(fwd_packet));
            break;
        default:
            throw std::runtime_error("unhandeld broadcast forwarding algorithm");
            break;
    }

    if (next_hop.valid()) {
        execute_media_procedures(m_mib.itsGnIfType);
        std::unique_ptr<Pdu> pdu;
        std::unique_ptr<DownPacket> payload;
        std::tie(pdu, payload) = next_hop.data();

        const MacAddress& mac = next_hop.mac();
        dcc::DataRequest request;
        request.destination = mac;
        request.source = m_local_position_vector.gn_addr.mid();
        request.dcc_profile = dcc::Profile::DP3;
        request.ether_type = geonet::ether_type;
        request.lifetime = std::chrono::seconds(pdu->basic().lifetime.decode() / units::si::seconds);

        pass_down(request, std::move(pdu), std::move(payload));
    }
}

void Router::flush_forwarding_buffer(PacketBuffer& buffer)
{
    dcc::DataRequest dcc_request;
    dcc_request.source = m_local_position_vector.gn_addr.mid();
    dcc_request.ether_type = geonet::ether_type;

    auto packets = buffer.flush(m_runtime.now());
    for (auto& packet : packets) {
        std::unique_ptr<Pdu> pdu;
        std::unique_ptr<DownPacket> payload;
        std::tie(pdu, payload) = packet.data();
        const auto tc = pdu->common().traffic_class;
        dcc_request.destination = packet.mac();
        dcc_request.dcc_profile = map_tc_onto_profile(tc);
        dcc_request.lifetime = std::chrono::seconds(pdu->basic().lifetime.decode() / units::si::seconds);
        pass_down(dcc_request, std::move(pdu), std::move(payload));
    }
}

void Router::flush_broadcast_forwarding_buffer()
{
    flush_forwarding_buffer(m_bc_forward_buffer);
}

void Router::flush_unicast_forwarding_buffer()
{
    flush_forwarding_buffer(m_uc_forward_buffer);
}

void Router::detect_duplicate_address(const Address& addr_so)
{
    // EN 302 636-4-1 V1.2.1 9.2.1.5: DAD is only applied for AUTO
    if (m_mib.itsGnLocalAddrConfMethod == AddrConfMethod::AUTO) {
        if (addr_so == m_local_position_vector.gn_addr) {
            MacAddress random_mac_addr;
            std::uniform_int_distribution<unsigned> octet_dist;
            for (auto& octet : random_mac_addr.octets) {
                octet = octet_dist(m_random_gen);
            }

            m_local_position_vector.gn_addr.mid(random_mac_addr);
        }
    }
}

std::unique_ptr<ShbPdu> Router::create_shb_pdu(const ShbDataRequest& request)
{
    std::unique_ptr<ShbPdu> pdu { new ShbPdu(request, m_mib) };
    pdu->common().header_type = HeaderType::TSB_SINGLE_HOP;
    pdu->extended().source_position = m_local_position_vector;
    return pdu;
}

std::unique_ptr<BeaconPdu> Router::create_beacon_pdu()
{
    std::unique_ptr<BeaconPdu> pdu { new BeaconPdu(m_mib) };
    pdu->basic().hop_limit = 1;
    pdu->common().next_header = NextHeaderCommon::ANY;
    pdu->common().header_type = HeaderType::BEACON;
    pdu->common().maximum_hop_limit = 1;
    // TODO: Shall we set traffic class to another DCC profile than DP0?
    pdu->extended().source_position = m_local_position_vector;
    return pdu;
}

std::unique_ptr<GbcPdu> Router::create_gbc_pdu(const GbcDataRequest& request)
{
    std::unique_ptr<GbcPdu> pdu { new GbcPdu(request, m_mib) };
    pdu->common().header_type = gbc_header_type(request.destination);
    pdu->extended().sequence_number = m_local_sequence_number++;
    pdu->extended().source_position = m_local_position_vector;
    pdu->extended().destination(request.destination);
    return pdu;
}

Router::DownPacketPtr Router::encap_packet(security::Profile profile, Pdu& pdu, DownPacketPtr packet)
{
    security::EncapRequest encap_request;

    DownPacket sec_payload;
    sec_payload[OsiLayer::Network] = SecuredPdu(pdu);
    sec_payload.merge(*packet, OsiLayer::Transport, max_osi_layer());
    encap_request.plaintext_payload = std::move(sec_payload);
    encap_request.security_profile = profile;

    if (m_security_entity) {
        security::EncapConfirm confirm = m_security_entity->encapsulate_packet(std::move(encap_request));
        pdu.secured(std::move(confirm.sec_packet));
    } else {
        throw std::runtime_error("security entity unavailable");
    }

    assert(size(*packet, OsiLayer::Transport, max_osi_layer()) == 0);
    assert(pdu.basic().next_header == NextHeaderBasic::SECURED);
    return packet;
}

} // namespace geonet
} // namespace vanetza
