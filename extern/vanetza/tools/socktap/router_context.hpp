#ifndef ROUTER_CONTEXT_HPP_KIPUYBY2
#define ROUTER_CONTEXT_HPP_KIPUYBY2

#include "dcc_passthrough.hpp"
#include <vanetza/btp/port_dispatcher.hpp>
#include <vanetza/geonet/mib.hpp>
#include <vanetza/geonet/router.hpp>
#include <vanetza/net/cohesive_packet.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/generic/raw_protocol.hpp>
#include <array>
#include <list>
#include <memory>

class Application;
class PositionProvider;
class TimeTrigger;

class RouterContext
{
public:
    RouterContext(boost::asio::generic::raw_protocol::socket&, const vanetza::geonet::MIB&, TimeTrigger&, PositionProvider&, vanetza::security::SecurityEntity&);
    ~RouterContext();
    void enable(Application*);

    /**
     * Allow/disallow transmissions without GNSS position fix
     *
     * \param flag true if transmissions shall be dropped when no GNSS position fix is available
     */
    void require_position_fix(bool flag);

private:
    void do_receive();
    void on_read(const boost::system::error_code&, std::size_t);
    void pass_up(vanetza::CohesivePacket&&);
    void log_packet_drop(vanetza::geonet::Router::PacketDropReason);
    void update_position_vector();
    void update_packet_flow(const vanetza::geonet::LongPositionVector&);

    vanetza::geonet::MIB mib_;
    vanetza::geonet::Router router_;
    boost::asio::generic::raw_protocol::socket& socket_;
    TimeTrigger& trigger_;
    PositionProvider& positioning_;
    vanetza::btp::PortDispatcher dispatcher_;
    std::unique_ptr<DccPassthrough> request_interface_;
    vanetza::ByteBuffer receive_buffer_;
    boost::asio::generic::raw_protocol::endpoint receive_endpoint_;
    std::list<Application*> applications_;
    bool require_position_fix_ = false;
};

#endif /* ROUTER_CONTEXT_HPP_KIPUYBY2 */
