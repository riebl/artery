#include "artery/testbed/CubeConnection.h"
#include "nfiniity_cube_radio.pb.h"
#include <omnetpp/clog.h>
#include <vanetza/access/ethertype.hpp>

using namespace omnetpp;

namespace artery
{

CubeConnection::CubeConnection(const boost::asio::ip::address& addr, int rxPort, int txPort, ReceivePacketHandler handler) :
    mReceivePacketHandler(handler),
    mIoWorkGuard(boost::asio::make_work_guard(mIoContext)),
    mTxEndpoint(addr, txPort),
    mRxEndpoint(boost::asio::ip::udp::v4(), rxPort),
    mTxSocket(mIoContext),
    mRxSocket(mIoContext, mRxEndpoint) /*< opens and binds to given endpoint */
{
    mTxSocket.async_connect(mTxEndpoint, [](const boost::system::error_code& ec) {
        if (ec) {
            EV_ERROR << "Failed to establish CubeConnection: " << ec.message() << std::endl;
        } else {
            EV_INFO << "CubeConnection established" << std::endl;
        }
    });

    mIoThread = std::thread([this]() {
        mIoContext.run();
    });
}

CubeConnection::~CubeConnection()
{
    mIoContext.stop();
    mIoThread.join();
}

void CubeConnection::receivePacket()
{
    auto parse = [this](boost::system::error_code ec, std::size_t bytes) {
        if (!ec) {
            GossipMessage message;
            if (message.ParseFromArray(mRxBuffer.data(), bytes) &&
                message.kind_case() == GossipMessage::KindCase::kLinklayerRx) {
                auto& rx = message.linklayer_rx();
  
                vanetza::access::DataRequest request;
                auto src_addr_len = std::min(rx.source().size(), vanetza::MacAddress::length_bytes);
                std::copy_n(rx.source().begin(), src_addr_len, request.source_addr.octets.begin());
                auto dst_addr_len = std::min(rx.destination().size(), vanetza::MacAddress::length_bytes);
                std::copy_n(rx.destination().begin(), dst_addr_len, request.destination_addr.octets.begin());
                request.access_category = vanetza::access::AccessCategory::BE;
                request.ether_type = vanetza::access::ethertype::GeoNetworking;

                vanetza::ByteBuffer buffer { rx.payload().begin(), rx.payload().end() };
                auto payload = std::make_unique<vanetza::CohesivePacket>(std::move(buffer), vanetza::OsiLayer::Network);

                EV_INFO << "received packet over CubeConnection" << std::endl;
                mReceivePacketHandler(request, std::move(payload));
            }

            // receive next packet
            receivePacket();
        } else {
            EV_ERROR << "receiving packet over CubeConnection failed: " << ec.message() << std::endl;
        }
    };

    mRxSocket.async_receive_from(boost::asio::buffer(mRxBuffer), mRxEndpoint, parse);
}

void CubeConnection::sendPacket(const vanetza::MacAddress& source, const vanetza::MacAddress& destination, const vanetza::byte_view_range& data)
{
    CommandRequest command;
    LinkLayerTransmission* tx = command.mutable_linklayer_tx();
    tx->set_source(source.octets.data(), source.octets.size());
    tx->set_destination(destination.octets.data(), destination.octets.size());
    tx->set_priority(LinkLayerPriority::BEST_EFFORT);
    tx->set_payload(data.data(), data.size());
    
    auto buffer = std::make_shared<std::string>(command.SerializeAsString());
    mTxSocket.async_send(boost::asio::buffer(*buffer), [buffer](const boost::system::error_code& ec, std::size_t) {
        if (ec) {
            EV_ERROR << "sending packet over CubeConnection failed: " << ec.message() << std::endl;;
        } else {
            EV_INFO << "sent packet over CubeConnection" << std::endl;
        }
    });
}

} // namespace artery
