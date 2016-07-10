#include "artery/inet/VanetRx.h"
#include <algorithm>

Define_Module(VanetRx)

const simsignal_t VanetRx::ChannelLoadSignal = cComponent::registerSignal("ChannelLoad");

VanetRx::VanetRx()
{
}

VanetRx::~VanetRx()
{
    cancelAndDelete(channelReportTrigger);
}

void VanetRx::initialize()
{
    Rx::initialize();
    channelLoadSamples.resize(12500);
    channelLoadLastUpdate = simTime();
    channelBusyRatio = 0.0;
    channelReportInterval = simtime_t { 100, SIMTIME_MS };
    channelReportTrigger = new cMessage("report CL");
    scheduleAt(simTime() + channelReportInterval, channelReportTrigger);

    WATCH(channelBusyRatio);
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
        std::size_t fillSamples = updateDelta / symbolPeriod;
        std::fill_n(std::back_inserter(channelLoadSamples), fillSamples, !mediumFree);
        channelLoadLastUpdate = simTime();
    }

    Rx::recomputeMediumFree();
}

void VanetRx::reportChannelLoad()
{
    const auto busy = std::count(channelLoadSamples.begin(), channelLoadSamples.end(), true);
    const auto total = channelLoadSamples.size();
    channelBusyRatio = busy;
    channelBusyRatio /= total;

    emit(ChannelLoadSignal, channelBusyRatio);
}
