#include "artery/application/NetworkInterface.h"
#include "artery/application/TransportDispatcher.h"

namespace artery
{

NetworkInterface::NetworkInterface(Router& router, IDccEntity& dcc, const TransportDispatcher& dispatcher) :
    channel(0), mRouter(router), mDccEntity(dcc), mTransportHandler(dispatcher, *this)
{
}

NetworkInterface::TransportHandler::TransportHandler(const TransportDispatcher& dispatcher, const NetworkInterface& net) :
    mTransportDispatcher(dispatcher), mNetworkInterface(net)
{
}

void NetworkInterface::TransportHandler::indicate(const vanetza::geonet::DataIndication& ind, std::unique_ptr<vanetza::UpPacket> packet)
{
    mTransportDispatcher.indicate(ind, std::move(packet), mNetworkInterface);
}

} // namespace artery
