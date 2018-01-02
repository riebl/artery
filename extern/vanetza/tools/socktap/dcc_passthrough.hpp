#ifndef DCC_PASSTHROUGH_HPP_GSDFESAE
#define DCC_PASSTHROUGH_HPP_GSDFESAE

#include "time_trigger.hpp"
#include <vanetza/dcc/data_request.hpp>
#include <vanetza/dcc/interface.hpp>
#include <vanetza/net/ethernet_header.hpp>
#include <vanetza/net/cohesive_packet.hpp>
#include <boost/asio/generic/raw_protocol.hpp>
#include <iostream>

class DccPassthrough : public vanetza::dcc::RequestInterface
{
public:
    DccPassthrough(boost::asio::generic::raw_protocol::socket& socket, TimeTrigger& trigger);

    void request(const vanetza::dcc::DataRequest& request, std::unique_ptr<vanetza::ChunkPacket> packet);

    void allow_packet_flow(bool allow);

    bool allow_packet_flow();

private:
    static constexpr std::size_t layers_ = num_osi_layers(vanetza::OsiLayer::Physical, vanetza::OsiLayer::Application);
    boost::asio::generic::raw_protocol::socket& socket_;
    std::array<vanetza::ByteBuffer, layers_> buffers_;
    TimeTrigger& trigger_;
    bool allow_packet_flow_ = true;
};

#endif /* DCC_PASSTHROUGH_HPP_GSDFESAE */
