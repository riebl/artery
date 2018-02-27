#include "artery/inet/VanetRx.h"
#include <cstdint>

Define_Module(VanetRx)

const simsignal_t VanetRx::ChannelLoadSignal = cComponent::registerSignal("ChannelLoad");

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
        channelLoadSamples.resize(12500);
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
        channelLoadSamples.insert(channelLoadSamples.end(), fillSamples, !mediumFree);
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
