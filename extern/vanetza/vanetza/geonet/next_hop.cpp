#include "next_hop.hpp"
#include <cassert>

namespace vanetza
{
namespace geonet
{

NextHop::NextHop() : m_state(State::DISCARDED)
{
}

bool NextHop::discarded() const
{
    return m_state == State::DISCARDED;
}

bool NextHop::buffered() const
{
    return m_state == State::BUFFERED;
}

bool NextHop::valid() const
{
    return m_state == State::VALID;
}

void NextHop::state(State s)
{
    m_state = s;
}

const MacAddress& NextHop::mac() const
{
    return m_destination;
}

void NextHop::mac(const MacAddress& addr)
{
    m_destination = addr;
}

std::tuple<std::unique_ptr<Pdu>, std::unique_ptr<DownPacket>>
NextHop::data()
{
    assert(m_pdu && m_payload);
    // set state to DISCARDED, so further valid() calls fail
    state(NextHop::State::DISCARDED);
    return std::make_tuple(std::move(m_pdu), std::move(m_payload));
}

void NextHop::data(std::unique_ptr<Pdu> pdu, std::unique_ptr<DownPacket> payload)
{
    m_pdu = std::move(pdu);
    m_payload = std::move(payload);
}

} // namespace geonet
} // namespace vanetza

