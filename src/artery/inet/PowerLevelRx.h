/*
* Artery V2X Simulation Framework
* Copyright 2019 Raphael Riebl
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#ifndef ARTERY_POWERLEVELRX_H_1N3KJPOI
#define ARTERY_POWERLEVELRX_H_1N3KJPOI

#include "artery/nic/ChannelLoadSampler.h"
#include "inet/linklayer/ieee80211/mac/Rx.h"
#include <omnetpp/clistener.h>

// forward declarations
namespace inet {
namespace physicallayer {
    class ICommunicationCache;
    class IRadio;
} // namespace physicallayer
} // namespace inet

namespace artery
{

/**
 * PowerLevelRx implements Clear Channel Assessment (CCA) with signal and noise thresholds.
 *
 * This allows to use receivers with better sensitivity than the minimum required by the IEEE 802.11 standard
 * while keeping CCA behaviour at a stable level.
 */
class PowerLevelRx : public inet::ieee80211::Rx, public omnetpp::cListener
{
public:
    PowerLevelRx();
    ~PowerLevelRx();

    static const omnetpp::simsignal_t ChannelLoadSignal;

protected:
    void initialize(int stage) override;
    void handleMessage(omnetpp::cMessage*) override;
    void recomputeMediumFree() override;

    void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t, omnetpp::cObject*, omnetpp::cObject*) override;

private:
    inet::physicallayer::IRadio* mRadio = nullptr;
    inet::physicallayer::ICommunicationCache* mCommunicationCache = nullptr;

    omnetpp::simtime_t mChannelReportInterval;
    omnetpp::cMessage* mChannelReportTrigger;
    ChannelLoadSampler mChannelLoadSampler;

    inet::W mBackgroundNoise;
    inet::W mCbrThreshold;
    inet::W mCcaSignalThreshold;
    inet::W mCcaNoiseThreshold;
    bool mCbrWithTx = false;
};

} // namespace artery

#endif /* ARTERY_POWERLEVELRX_H_1N3KJPOI */
