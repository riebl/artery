#ifndef ARTERY_VANETRX_H_KTVCYSUX
#define ARTERY_VANETRX_H_KTVCYSUX

#include "inet/linklayer/ieee80211/mac/Rx.h"
#include <deque>
#include <tuple>

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
    omnetpp::simtime_t channelLoadLastUpdate;
    std::deque<std::tuple<unsigned, bool>> channelLoadSamples;
    double channelBusyRatio;
};

} // namespace artery

#endif /* ARTERY_VANETRX_H_KTVCYSUX */

