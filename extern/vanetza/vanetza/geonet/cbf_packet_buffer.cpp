#include <vanetza/common/runtime.hpp>
#include <vanetza/geonet/address.hpp>
#include <vanetza/geonet/cbf_packet_buffer.hpp>
#include <vanetza/geonet/pdu.hpp>
#include <vanetza/units/time.hpp>
#include <cassert>
#include <iterator>

namespace vanetza
{
namespace geonet
{

CbfPacketIdentifier identifier(const CbfPacket& packet)
{
    return identifier(packet.source(), packet.sequence_number());
}

CbfPacketIdentifier identifier(const Address& source, SequenceNumber sn)
{
    return std::make_tuple(source, sn);
}


CbfPacket::CbfPacket(PduPtr pdu, PayloadPtr payload) :
    m_pdu(std::move(pdu)), m_payload(std::move(payload)), m_counter(1)
{
}

const Address& CbfPacket::source() const
{
    assert(m_pdu);
    return m_pdu->extended().source_position.gn_addr;
}

SequenceNumber CbfPacket::sequence_number() const
{
    assert(m_pdu);
    return m_pdu->extended().sequence_number;
}

Lifetime& CbfPacket::lifetime()
{
    assert(m_pdu);
    return m_pdu->basic().lifetime;
}

std::size_t CbfPacket::length() const
{
    const auto pdu_length = m_pdu ? get_length(*m_pdu) : 0;
    const auto payload_length = m_payload ? m_payload->size(OsiLayer::Transport, max_osi_layer()) : 0;
    return pdu_length + payload_length;
}

CbfPacket::Data::Data(CbfPacket&& packet) :
    pdu(std::move(packet.m_pdu)), payload(std::move(packet.m_payload))
{
}


CbfPacketBuffer::CbfPacketBuffer(Runtime& rt, TimerCallback cb, std::size_t bytes) :
    m_runtime(rt), m_capacity_bytes(bytes), m_stored_bytes(0),
    m_timer_callback(cb)
{
}

bool CbfPacketBuffer::try_drop(const Address& src, SequenceNumber sn)
{
    bool packet_dropped = false;

    auto& id_map = m_timers.right;
    auto found = id_map.find(identifier(src, sn));
    if (found != id_map.end()) {
        auto& packet = found->info;
        m_stored_bytes -= packet->length();
        m_packets.erase(packet);
        auto& timer_map = m_timers.left;
        auto successor = timer_map.erase(m_timers.project_left(found));
        if (successor == timer_map.begin() && !timer_map.empty()) {
            // erased timer was scheduled one, reschedule timer trigger
            schedule_timer();
        }
        packet_dropped = true;
    }

    assert(m_packets.size() == m_timers.size());
    return packet_dropped;
}

void CbfPacketBuffer::enqueue(CbfPacket&& packet, Clock::duration timeout)
{
    assert(timeout > Clock::duration::zero());
    m_stored_bytes += packet.length();

    // do head drop if necessary
    while (m_stored_bytes > m_capacity_bytes && !m_packets.empty()) {
        m_stored_bytes -= m_packets.front().length();
        m_timers.right.erase(identifier(m_packets.front()));
        m_packets.pop_front();
    }

    Timer timer = { m_runtime, timeout };
    Identifier id = identifier(packet);
    m_packets.emplace_back(std::move(packet));
    using timer_value = timer_bimap::value_type;
    auto insertion = m_timers.insert(timer_value { timer, id, std::prev(m_packets.end()) });
    if (!insertion.second) {
        m_stored_bytes -= m_packets.back().length();
        m_packets.pop_back();
        assert(m_packets.size() == m_timers.size());
        throw std::runtime_error("Illegal insertion of duplicate CBF packet");
    } else if (m_timers.project_left(insertion.first) == m_timers.left.begin()) {
        // enqueued packet expires first, reschedule timer trigger
        schedule_timer();
    }
    assert(m_packets.size() == m_timers.size());
}

boost::optional<CbfPacket> CbfPacketBuffer::fetch(const Address& src, SequenceNumber sn)
{
    boost::optional<CbfPacket> packet;
    auto& id_map = m_timers.right;
    auto found = id_map.find(identifier(src, sn));
    if (found != id_map.end()) {
        const Timer& timer = found->second;
        CbfPacket& cbf_packet = *found->info;
        bool valid_packet = reduce_lifetime(timer, cbf_packet);
        m_stored_bytes -= cbf_packet.length();
        if (valid_packet) {
            packet.emplace(std::move(cbf_packet));
        }
        m_packets.erase(found->info);
        auto& timer_map = m_timers.left;
        auto successor = timer_map.erase(m_timers.project_left(found));
        if (successor == timer_map.begin() && !timer_map.empty()) {
            // erased timer was scheduled one, reschedule timer trigger
            schedule_timer();
        }
    }
    return packet;
}

const CbfPacket* CbfPacketBuffer::find(const Address& src, SequenceNumber sn) const
{
    Identifier id = identifier(src, sn);
    const auto& id_map = m_timers.right;
    auto found = id_map.find(id);
    return found != id_map.end() ? &(*found->info) : nullptr;
}

void CbfPacketBuffer::flush()
{
    // fetch all expired timers
    const Timer now { m_runtime, std::chrono::seconds(0) };
    auto end = m_timers.left.upper_bound(now);
    for (auto it = m_timers.left.begin(); it != end;) {
        // reduce LT by queuing time
        const Timer& timer = it->first;
        CbfPacket& packet = *it->info;
        bool valid_packet = reduce_lifetime(timer, packet);
        m_stored_bytes -= packet.length();
        if (valid_packet) {
            CbfPacket::Data data(std::move(packet));
            m_timer_callback(std::move(data));
        }

        m_packets.erase(it->info);
        it = m_timers.left.erase(it);
    }

    // schedule timer if not empty
    if (!m_timers.empty()) {
        schedule_timer();
    }
}

bool CbfPacketBuffer::reduce_lifetime(const Timer& timer, CbfPacket& packet) const
{
    const auto queuing_time = m_runtime.now() - timer.start;
    const auto clock_lifetime = units::clock_cast(packet.lifetime().decode());
    bool valid_packet = false;
    if (queuing_time < clock_lifetime) {
        packet.lifetime().encode(units::clock_cast(clock_lifetime - queuing_time));
        valid_packet = true;
    }
    return valid_packet;
}

void CbfPacketBuffer::schedule_timer()
{
    const static std::string timer_name = "geonet.cbf.timer";
    assert(!m_timers.empty());
    m_runtime.cancel(timer_name);
    Runtime::Callback cb = [this](Clock::time_point) { flush(); };
    m_runtime.schedule(m_timers.left.begin()->first.expiry, cb, timer_name);
}


CbfPacketBuffer::Timer::Timer(const Runtime& rt, Clock::duration timeout) :
    expiry(rt.now() + timeout), start(rt.now())
{
}

bool CbfPacketBuffer::Timer::operator<(const Timer& other) const
{
    return this->expiry < other.expiry;
}

} // namespace geonet
} // namespace vanetza

namespace std
{

using Identifier = vanetza::geonet::CbfPacketIdentifier;
size_t hash<Identifier>::operator()(const Identifier& id) const
{
    using vanetza::geonet::Address;
    using vanetza::geonet::SequenceNumber;
    static_assert(tuple_size<Identifier>::value == 2, "Unexpected identifier tuple");

    std::size_t seed = 0;
    const Address& source = get<0>(id);
    boost::hash_combine(seed, std::hash<Address>()(source));
    const SequenceNumber& sn = get<1>(id);
    boost::hash_combine(seed, static_cast<SequenceNumber::value_type>(sn));
    return seed;
}

} // namespace std
