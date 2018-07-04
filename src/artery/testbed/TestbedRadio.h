/*
 * Artery V2X Simulation Framework
 * Copyright 2017-2018 Christina Obermaier
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_TESTBEDRADIO_H
#define ARTERY_TESTBEDRADIO_H

#include <inet/linklayer/common/MacAddress.h>
#include <inet/physicallayer/ieee80211/packetlevel/Ieee80211Radio.h>
#include <map>

namespace artery
{

class TestbedRadio : public inet::physicallayer::Ieee80211Radio
{
    public:
        virtual const std::map<inet::MacAddress, omnetpp::simtime_t>& getReachableNodes();

    protected:
        void initialize(int stage) override;
        void sendUp(inet::Packet* packet) override;
        virtual void updateReachableNodes();

    private:
        std::map<inet::MacAddress, omnetpp::simtime_t> mReachableNodes;
        omnetpp::simtime_t mReachableTime;
};

} // namespace artery

#endif /* ARTERY_TESTBEDRADIO_H */
