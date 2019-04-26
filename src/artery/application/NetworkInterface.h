#ifndef ARTERY_NETWORKINTERFACE_H_
#define ARTERY_NETWORKINTERFACE_H_

#include "artery/networking/IDccEntity.h"
#include "artery/utility/Channel.h"
#include <vanetza/geonet/transport_interface.hpp>

namespace artery
{

// forward declarations
class Router;
class TransportDispatcher;

/**
 * NetworkInterface interconnects the GeoNetworking Router and Middleware's services.
 */
class NetworkInterface
{
    public:
        NetworkInterface(Router& router, IDccEntity& entity, const TransportDispatcher& dispatcher);

        /**
         * TransportHandler is called by the GeoNetworking router when passing incoming packets upwards.
         * This interface is registered at vanetza::geonet::Router by Router module.
         */
        class TransportHandler : public vanetza::geonet::TransportInterface
        {
            public:
                TransportHandler(const TransportDispatcher& dispatcher, const NetworkInterface& net);
                TransportHandler(const TransportHandler&) = delete;
                TransportHandler& operator=(const TransportHandler&) = delete;
                void indicate(const vanetza::geonet::DataIndication& ind, std::unique_ptr<vanetza::UpPacket> packet) override;

            private:
                const TransportDispatcher& mTransportDispatcher;
                const NetworkInterface& mNetworkInterface;
        };

        TransportHandler& getTransportHandler() { return mTransportHandler; }
        Router& getRouter() const { return mRouter; }
        IDccEntity& getDccEntity() const { return mDccEntity; }

        ChannelNumber channel; /*< ITS channel served by this network interface (might change during lifetime) */

    private:
        Router& mRouter;
        IDccEntity& mDccEntity;
        TransportHandler mTransportHandler;
};

} // namespace artery

#endif /* ARTERY_NETWORKINTERFACE_H_ */
