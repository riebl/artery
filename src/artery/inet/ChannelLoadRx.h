#ifndef ARTERY_CHANNELLOADRX_H_C0YJMQTS
#define ARTERY_CHANNELLOADRX_H_C0YJMQTS

#include "artery/nic/ChannelLoadSampler.h"
#include "inet/linklayer/ieee80211/mac/Rx.h"

namespace artery
{

class ChannelLoadRx : public inet::ieee80211::Rx
{
public:
    ChannelLoadRx();
    ~ChannelLoadRx();

    static const omnetpp::simsignal_t ChannelLoadSignal;

protected:
    void initialize(int stage) override;
    void handleMessage(omnetpp::cMessage*) override;
    void recomputeMediumFree() override;

private:
    omnetpp::simtime_t mChannelReportInterval;
    omnetpp::cMessage* mChannelReportTrigger;
    ChannelLoadSampler mChannelLoadSampler;
    bool mCbrWithTx = false;
};

} // namespace artery

#endif /* ARTERY_CHANNELLOADRX_H_C0YJMQTS */

