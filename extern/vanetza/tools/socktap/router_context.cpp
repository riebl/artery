#include "application.hpp"
#include "ethernet_device.hpp"
#include "position_provider.hpp"
#include "router_context.hpp"
#include "time_trigger.hpp"
#include <vanetza/dcc/data_request.hpp>
#include <vanetza/dcc/interface.hpp>
#include <vanetza/net/ethernet_header.hpp>
#include <boost/asio/generic/raw_protocol.hpp>
#include <iostream>

namespace asio = boost::asio;
using boost::asio::generic::raw_protocol;
using namespace vanetza;

class DccPassthrough : public dcc::RequestInterface
{
public:
    DccPassthrough(raw_protocol::socket& socket, TimeTrigger& trigger) :
        socket_(socket), trigger_(trigger) {}

    void request(const dcc::DataRequest& request, std::unique_ptr<ChunkPacket> packet)
    {
        buffers_[0] = create_ethernet_header(request.destination, request.source, request.ether_type);
        for (auto& layer : osi_layer_range<OsiLayer::Network, OsiLayer::Application>()) {
            const auto index = distance(OsiLayer::Link, layer);
            packet->layer(layer).convert(buffers_[index]);
        }

        trigger_.schedule();

        std::array<asio::const_buffer, layers_> const_buffers;
        for (unsigned i = 0; i < const_buffers.size(); ++i) {
            const_buffers[i] = asio::buffer(buffers_[i]);
        }
        auto bytes_sent = socket_.send(const_buffers);
        std::cout << "sent packet to " << request.destination << " (" << bytes_sent << " bytes)\n";
    }

private:
    static constexpr std::size_t layers_ = num_osi_layers(OsiLayer::Link, OsiLayer::Application);
    raw_protocol::socket& socket_;
    std::array<ByteBuffer, layers_> buffers_;
    TimeTrigger& trigger_;
};

geonet::MIB configure_mib(const EthernetDevice& device)
{
    geonet::MIB mib;
    mib.itsGnLocalGnAddr.mid(device.address());
    mib.itsGnLocalAddrConfMethod = geonet::AddrConfMethod::MANAGED;
    mib.itsGnSecurity = false;
    return mib;
}

RouterContext::RouterContext(raw_protocol::socket& socket, const EthernetDevice& device, TimeTrigger& trigger, PositionProvider& positioning) :
    mib_(configure_mib(device)), router_(trigger.runtime(), mib_),
    socket_(socket), device_(device), trigger_(trigger), positioning_(positioning),
    request_interface_(new DccPassthrough(socket, trigger)),
    receive_buffer_(2048, 0x00), receive_endpoint_(socket_.local_endpoint())
{
    router_.packet_dropped = std::bind(&RouterContext::log_packet_drop, this, std::placeholders::_1);
    router_.set_address(mib_.itsGnLocalGnAddr);
    router_.set_access_interface(request_interface_.get());
    router_.set_transport_handler(geonet::UpperProtocol::BTP_B, &dispatcher_);
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
    std::cout << "Router dropped packet because of " << static_cast<int>(reason) << "\n";
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
        pass_up(CohesivePacket(std::move(buffer), OsiLayer::Link));
        do_receive();
    }
}

void RouterContext::pass_up(CohesivePacket&& packet)
{
    if (packet.size(OsiLayer::Link) < EthernetHeader::length_bytes) {
        std::cerr << "Router dropped invalid packet (too short)\n";
    } else {
        packet.set_boundary(OsiLayer::Link, EthernetHeader::length_bytes);
        auto link_range = packet[OsiLayer::Link];
        EthernetHeader hdr = decode_ethernet_header(link_range.begin(), link_range.end());
        if (hdr.source != mib_.itsGnLocalGnAddr.mid()) {
            std::cout << "received packet from " << hdr.source << " (" << packet.size() << " bytes)\n";
            std::unique_ptr<PacketVariant> up { new PacketVariant(std::move(packet)) };
            router_.indicate(std::move(up), hdr.source, hdr.destination);
            trigger_.schedule();
        }
    }
}

void RouterContext::enable(Application* app)
{
    app->router_ = &router_;
    dispatcher_.set_non_interactive_handler(app->port(), app);
}

void RouterContext::update_position_vector()
{
    router_.update(positioning_.current_position());
    vanetza::Runtime::Callback callback = [this](vanetza::Clock::time_point) { this->update_position_vector(); };
    vanetza::Clock::duration next = std::chrono::seconds(1);
    trigger_.runtime().schedule(next, callback);
    trigger_.schedule();
}
