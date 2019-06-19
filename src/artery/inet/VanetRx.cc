#include "artery/inet/VanetRx.h"
#include <cstdint>

using namespace omnetpp;

namespace artery
{

const simsignal_t VanetRx::ChannelLoadSignal = cComponent::registerSignal("ChannelLoad");

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
        channelLoadSamples.push_front(std::tuple<int,bool>(12500, false));
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
        channelLoadSamples.push_front(std::tuple<uint,bool>(fillSamples, !mediumFree));
        channelLoadLastUpdate = simTime();
    }

    Rx::recomputeMediumFree();
}

void VanetRx::reportChannelLoad()
{
    uint samples = 0;
    uint busy = 0;
    double channelBusyRatio = 0.0;

    /* iterate recorded busy periods in reverse */
    for (auto it = channelLoadSamples.begin(); it != channelLoadSamples.end();) {
        /* get values from tuples */
        uint sampleLength = std::get<0>(*it);
        bool mediumBusy = std::get<1>(*it);

        /* add length of list entry to total samples */
        samples += sampleLength;

        if (samples < cbrInterval) {
            if (mediumBusy) {
                /* increase busy time */
                busy += sampleLength;
            }
            ++it;
        }
        else {
            /* add remaining samples to to busy time*/
            busy += (cbrInterval - samples) * (int)mediumBusy;

            /* remove unused entries from list */
            it = channelLoadSamples.erase(it, channelLoadSamples.end());
        }
    }

    channelBusyRatio = busy;
    channelBusyRatio /= cbrInterval;

    emit(ChannelLoadSignal, channelBusyRatio);
}

} // namespace artery
