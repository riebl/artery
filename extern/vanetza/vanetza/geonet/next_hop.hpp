#ifndef NEXT_HOP_HPP_ON0AKMBY
#define NEXT_HOP_HPP_ON0AKMBY

#include <vanetza/geonet/packet.hpp>
#include <vanetza/geonet/pdu.hpp>
#include <vanetza/net/mac_address.hpp>
#include <memory>
#include <tuple>

namespace vanetza
{
namespace geonet
{

class NextHop
{
public:
    enum class State {
        VALID,
        DISCARDED,
        BUFFERED
    };

    NextHop();
    NextHop(NextHop&&) = default;
    NextHop& operator=(NextHop&&) = default;

    bool discarded() const;
    bool buffered() const;
    bool valid() const;
    void state(State);
    const MacAddress& mac() const;
    void mac(const MacAddress&);
    std::tuple<std::unique_ptr<Pdu>, std::unique_ptr<DownPacket>> data();
    void data(std::unique_ptr<Pdu>, std::unique_ptr<DownPacket>);

private:
    State m_state;
    MacAddress m_destination;
    std::unique_ptr<Pdu> m_pdu;
    std::unique_ptr<DownPacket> m_payload;
};

} // namespace geonet
} // namespace vanetza

#endif /* NEXT_HOP_HPP_ON0AKMBY */

