#include "artery/application/IndicationInterface.h"
#include "artery/application/NetworkInterface.h"
#include "artery/application/TappingInterface.h"
#include "artery/application/TransportDispatcher.h"
#include <omnetpp/clog.h>
#include <vanetza/btp/header.hpp>

using namespace vanetza;

namespace artery
{

void TransportDispatcher::indicate(const geonet::DataIndication& gn_ind, std::unique_ptr<UpPacket> packet, const  NetworkInterface& net) const
{
    if (gn_ind.upper_protocol == geonet::UpperProtocol::BTP_B && packet) {
        // parse BTP-B header
        btp::HeaderB hdr = btp::parse_btp_b(*packet);
        btp::DataIndication btp_ind(gn_ind, hdr);

        // indicate promiscuous listeners
        auto found_channel = mPromiscuousListeners.find(net.channel);
        if (found_channel != mPromiscuousListeners.end()) {
            for (TappingInterface* listener : found_channel->second) {
                listener->tap(btp_ind, *packet, net);
            }
        }

        // indicate regular listeners
        auto found_descriptor = mListeners.find(std::make_tuple(net.channel, btp_ind.destination_port.host()));
        if (found_descriptor != mListeners.end()) {
            unsigned pending = found_descriptor->second.size();
            for (IndicationInterface* listener : found_descriptor->second) {
                if (pending > 1) {
                    std::unique_ptr<vanetza::UpPacket> dup { new vanetza::UpPacket { *packet } };
                    listener->indicate(btp_ind, std::move(dup), net);
                } else {
                    listener->indicate(btp_ind, std::move(packet), net);
                }
                --pending;
            }
        }
    } else {
        EV_STATICCONTEXT
        EV_ERROR << "Cannot dispatch other packets than BTP-B\n";
    }
}

void TransportDispatcher::addListener(IndicationInterface* ifc, const TransportDescriptor& td)
{
    if (ifc) {
        mListeners[td].insert(ifc);
    }
}

void TransportDispatcher::addPromiscuousListener(TappingInterface* ifc, ChannelNumber ch)
{
    if (ifc) {
        mPromiscuousListeners[ch].insert(ifc);
    }
}

} // namespace artery
