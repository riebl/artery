#ifndef ARTERY_VANETRX_H_KTVCYSUX
#define ARTERY_VANETRX_H_KTVCYSUX

#include "artery/nic/ChannelLoadSampler.h"
#include "inet/linklayer/ieee80211/mac/Rx.h"

namespace artery
{

class VanetRx : public inet::ieee80211::Rx
{
public:
    VanetRx();
    ~VanetRx();

    static const omnetpp::simsignal_t ChannelLoadSignal;

protected:
    void initialize(int stage) override;
    void handleMessage(omnetpp::cMessage*) override;
    void recomputeMediumFree() override;
    virtual void reportChannelLoad();

private:
    omnetpp::simtime_t channelReportInterval;
    omnetpp::cMessage* channelReportTrigger;
    ChannelLoadSampler channelLoadSampler;
};

} // namespace artery

#endif /* ARTERY_VANETRX_H_KTVCYSUX */

