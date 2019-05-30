#ifndef ARTERY_TRANSPORTDISPATCHER_H_RBNQZIRF
#define ARTERY_TRANSPORTDISPATCHER_H_RBNQZIRF

#include "artery/application/TransportDescriptor.h"
#include "artery/utility/Channel.h"
#include <vanetza/geonet/data_indication.hpp>
#include <map>
#include <set>

namespace artery
{

// forward declarations
class NetworkInterface;
class IndicationInterface;
class TappingInterface;

/**
 * TransportDispatcher forwards incoming BTP-B packets to matching listeners, i.e. ITS-G5 services
 */
class TransportDispatcher
{
    public:
        /**
         * NetworkInterface's TransportHandler will pass incoming packets to dispatcher via this method.
         */
        void indicate(const vanetza::geonet::DataIndication&, std::unique_ptr<vanetza::UpPacket>, const NetworkInterface&) const;

        /**
         * Add an "ordinary" listener
         *
         * \param ifc listener implements the IndicationInterface for receiving packets
         * \param td a TransportDescriptor describing channel and port thi listener is interested in
         */
        void addListener(IndicationInterface* ifc, const TransportDescriptor& td);

        /**
         * Add a "promiscuous" listener
         *
         * These listeners do not care about the port, they listen to everything on a particular channel.
         *
         * \param ifc listener implements the TappingInterface for receiving packets
         * \param ch listen on this channel
         */
        void addPromiscuousListener(TappingInterface*, ChannelNumber ch = channel::CCH);

    private:
        std::map<TransportDescriptor, std::set<IndicationInterface*>> mListeners;
        std::map<ChannelNumber, std::set<TappingInterface*>> mPromiscuousListeners;
};

} // namespace artery

#endif /* ARTERY_TRANSPORTDISPATCHER_H_RBNQZIRF */

