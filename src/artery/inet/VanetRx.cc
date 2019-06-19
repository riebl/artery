#include "artery/inet/VanetRx.h"
#include <cstdint>

using namespace omnetpp;

namespace artery
{

const simsignal_t VanetRx::ChannelLoadSignal = cComponent::registerSignal("ChannelLoad");
static const unsigned cbrIntervalSamples = 12500;

Define_Module(VanetRx)

VanetRx::VanetRx()
{
}

VanetRx::~VanetRx()
{
    cancelAndDelete(channelReportTrigger);
}

void VanetRx::initialize(int stage)
{
    Rx::initialize(stage);
    if (stage == 0) {
        channelLoadSamples.emplace_front(cbrIntervalSamples, false);
        channelLoadLastUpdate = simTime();
        channelBusyRatio = 0.0;
        channelReportInterval = simtime_t { 100, SIMTIME_MS };
        channelReportTrigger = new cMessage("report CL");
        scheduleAt(simTime() + channelReportInterval, channelReportTrigger);

        WATCH(channelBusyRatio);
    }
}

void VanetRx::handleMessage(cMessage* msg)
{
    if (msg == channelReportTrigger) {
        reportChannelLoad();
        scheduleAt(simTime() + channelReportInterval, channelReportTrigger);
    } else {
        Rx::handleMessage(msg);
    }
}

void VanetRx::recomputeMediumFree()
{
    static const simtime_t symbolPeriod { 8, SIMTIME_US };
    const auto updateDelta = simTime() - channelLoadLastUpdate;
    if (updateDelta >= symbolPeriod) {
        const std::size_t fillSamples = updateDelta / symbolPeriod;
        channelLoadSamples.emplace_front(fillSamples, !mediumFree);
        channelLoadLastUpdate = simTime();
    }

    Rx::recomputeMediumFree();
}

void VanetRx::reportChannelLoad()
{
    unsigned samples = 0;
    unsigned busy = 0;

    // iterate recorded busy periods (begin with most recent period)
    for (auto it = channelLoadSamples.begin(); it != channelLoadSamples.end();) {
        // get values from tuples
        unsigned sampleLength = std::get<0>(*it);
        bool mediumBusy = std::get<1>(*it);

        // add length of list entry to total samples
        samples += sampleLength;

        if (samples < cbrIntervalSamples) {
            if (mediumBusy) {
                // increase busy time by full sample length
                busy += sampleLength;
            }
            ++it;
        } else {
            if (mediumBusy) {
                // increase busy time up to boundary of sampling interval
                busy += sampleLength - (samples - cbrIntervalSamples);
            }

            // remove obsolete periods
            it = channelLoadSamples.erase(it, channelLoadSamples.end());
        }
    }

    channelBusyRatio = busy;
    channelBusyRatio /= cbrIntervalSamples;
    emit(ChannelLoadSignal, channelBusyRatio);
}

} // namespace artery
