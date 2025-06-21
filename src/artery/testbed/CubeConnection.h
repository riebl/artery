/*
 * Artery V2X Simulation Framework
 * Copyright 2024 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_CUBECONNECTION_H
#define ARTERY_CUBECONNECTION_H

#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/udp.hpp>
#include <vanetza/access/data_request.hpp>
#include <vanetza/common/byte_view.hpp>
#include <vanetza/net/cohesive_packet.hpp>
#include <vanetza/net/mac_address.hpp>
#include <array>
#include <functional>
#include <thread>

namespace artery
{

class CubeConnection
{
public:
    /**
     * \brief Callback for packets received by CUBE for dissemination in simulation
     */
    using ReceivePacketHandler = std::function<void(const vanetza::access::DataRequest&, std::unique_ptr<vanetza::CohesivePacket>)>;

    /**
     * \brief Construct a new connection to a CUBE device
     * \param addr network address of CUBE device
     * \param rxPort port for receiving packets
     * \param txPort port for transmitting packets
     * \param handler callback for received packets
     */
    CubeConnection(const boost::asio::ip::address& addr, int rxPort, int txPort, ReceivePacketHandler handler);
    ~CubeConnection();

    /**
     * \brief Send a link-layer packet to CUBE for transmission
     *
     * \param source sender's MAC address
     * \param destination destination's MAC address
     * \param data link-layer payload to be sent
     */
    void sendPacket(const vanetza::MacAddress& source, const vanetza::MacAddress& destination, const vanetza::byte_view_range& data);

private:
    void receivePacket();

    ReceivePacketHandler mReceivePacketHandler;
    std::thread mIoThread;
    boost::asio::io_context mIoContext;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> mIoWorkGuard;
    boost::asio::ip::udp::endpoint mTxEndpoint;
    boost::asio::ip::udp::endpoint mRxEndpoint;
    boost::asio::ip::udp::socket mTxSocket;
    boost::asio::ip::udp::socket mRxSocket;
    std::array<uint8_t, 4096> mRxBuffer;
};

} // namespace artery

#endif /* ARTERY_CUBECONNECTION_H */
