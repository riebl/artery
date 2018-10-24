#ifndef ARTERY_VANETRX_H_KTVCYSUX
#define ARTERY_VANETRX_H_KTVCYSUX

#include "inet/linklayer/ieee80211/mac/Rx.h"
#include <boost/circular_buffer.hpp>

namespace artery
{

class VanetRx : public inet::ieee80211::Rx
{
public:
    VanetRx();
    ~VanetRx();

    static const simsignal_t ChannelLoadSignal;

protected:
    void initialize(int stage) override;
    void handleMessage(cMessage*) override;
    void recomputeMediumFree() override;
    virtual void reportChannelLoad();

private:
    simtime_t channelReportInterval;
    cMessage* channelReportTrigger;
    simtime_t channelLoadLastUpdate;
    boost::circular_buffer<bool> channelLoadSamples;
    double channelBusyRatio;
};

} // namespace artery

#endif /* ARTERY_VANETRX_H_KTVCYSUX */

