#include "application.hpp"
#include "dcc_passthrough.hpp"
#include "ethernet_device.hpp"
#include "position_provider.hpp"
#include "router_context.hpp"
#include "time_trigger.hpp"
#include <vanetza/dcc/data_request.hpp>
#include <vanetza/dcc/interface.hpp>
#include <vanetza/net/ethernet_header.hpp>
#include <boost/asio/generic/raw_protocol.hpp>
#include <iostream>

#ifdef SOCKTAP_WITH_COHDA_SDK
#include "cohda.hpp"
#include <mk2mac-api-types.h>
#endif

namespace asio = boost::asio;
using boost::asio::generic::raw_protocol;
using namespace vanetza;

RouterContext::RouterContext(raw_protocol::socket& socket, const geonet::MIB& mib, TimeTrigger& trigger, PositionProvider& positioning, vanetza::security::SecurityEntity& security_entity) :
    mib_(mib), router_(trigger.runtime(), mib_),
    socket_(socket), trigger_(trigger), positioning_(positioning),
    request_interface_(new DccPassthrough(socket, trigger)),
    receive_buffer_(2048, 0x00), receive_endpoint_(socket_.local_endpoint())
{
    router_.packet_dropped = std::bind(&RouterContext::log_packet_drop, this, std::placeholders::_1);
    router_.set_address(mib_.itsGnLocalGnAddr);
    router_.set_access_interface(request_interface_.get());
    router_.set_transport_handler(geonet::UpperProtocol::BTP_B, &dispatcher_);
    router_.set_security_entity(&security_entity);
    update_position_vector();

    do_receive();
    trigger_.schedule();
}

RouterContext::~RouterContext()
{
    for (auto* app : applications_) {
        app->router_ = nullptr;
    }
}

void RouterContext::log_packet_drop(geonet::Router::PacketDropReason reason)
{
    auto reason_string = stringify(reason);
    std::cout << "Router dropped packet because of " << reason_string << " (" << static_cast<int>(reason) << ")\n";
}

void RouterContext::do_receive()
{
    namespace sph = std::placeholders;
    socket_.async_receive_from(
            asio::buffer(receive_buffer_), receive_endpoint_,
            std::bind(&RouterContext::on_read, this, sph::_1, sph::_2));
}

void RouterContext::on_read(const boost::system::error_code& ec, std::size_t read_bytes)
{
    if (!ec) {
        ByteBuffer buffer(receive_buffer_.begin(), receive_buffer_.begin() + read_bytes);
        pass_up(CohesivePacket(std::move(buffer), OsiLayer::Physical));
        do_receive();
    }
}

void RouterContext::pass_up(CohesivePacket&& packet)
{
#ifdef SOCKTAP_WITH_COHDA_SDK
    if (packet.size(OsiLayer::Physical) < sizeof(tMK2RxDescriptor)) {
        std::cerr << "Router dropped invalid packet (too short for Cohda Rx Descriptor)\n";
        return;
    } else {
        packet.set_boundary(OsiLayer::Physical, sizeof(tMK2RxDescriptor));
        // information from Mk2RxDescriptor is currently ignored
    }
#else
    packet.set_boundary(OsiLayer::Physical, 0);
#endif

    if (packet.size(OsiLayer::Link) < EthernetHeader::length_bytes) {
        std::cerr << "Router dropped invalid packet (too short for Ethernet header)\n";
    } else {
        packet.set_boundary(OsiLayer::Link, EthernetHeader::length_bytes);
        auto link_range = packet[OsiLayer::Link];
        EthernetHeader hdr = decode_ethernet_header(link_range.begin(), link_range.end());
        if (hdr.source != mib_.itsGnLocalGnAddr.mid()) {
            std::cout << "received packet from " << hdr.source << " (" << packet.size() << " bytes)\n";
            std::unique_ptr<PacketVariant> up { new PacketVariant(std::move(packet)) };
            trigger_.schedule(); // ensure the clock is up-to-date for the security entity
            router_.indicate(std::move(up), hdr.source, hdr.destination);
            trigger_.schedule(); // schedule packet forwarding
        }
    }
}

void RouterContext::enable(Application* app)
{
    app->router_ = &router_;
    dispatcher_.set_non_interactive_handler(app->port(), app);
}

void RouterContext::require_position_fix(bool flag)
{
    require_position_fix_ = flag;
    update_packet_flow(router_.get_local_position_vector());
}

void RouterContext::update_position_vector()
{
    auto position = positioning_.current_position();
    router_.update(position);
    vanetza::Runtime::Callback callback = [this](vanetza::Clock::time_point) { this->update_position_vector(); };
    vanetza::Clock::duration next = std::chrono::seconds(1);
    trigger_.runtime().schedule(next, callback);
    trigger_.schedule();

    update_packet_flow(position);
}

void RouterContext::update_packet_flow(const geonet::LongPositionVector& lpv)
{
    if (require_position_fix_) {
        // Skip all requests until a valid GPS position is available
        request_interface_->allow_packet_flow(lpv.position_accuracy_indicator);
    } else {
        request_interface_->allow_packet_flow(true);
    }
}

