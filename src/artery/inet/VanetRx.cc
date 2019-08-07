#include "artery/inet/VanetRx.h"

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
        mCbrWithTx = par("cbrWithTx");
        channelReportInterval = simtime_t { 100, SIMTIME_MS };
        channelReportTrigger = new cMessage("report CL");
        scheduleAt(simTime() + channelReportInterval, channelReportTrigger);

        WATCH(channelLoadSampler);
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
    Rx::recomputeMediumFree();

    using ReceptionState = inet::physicallayer::IRadio::ReceptionState;
    if (mCbrWithTx) {
        channelLoadSampler.busy(!mediumFree);
    } else {
        channelLoadSampler.busy(!mediumFree && receptionState > ReceptionState::RECEPTION_STATE_IDLE);
    }
}

void VanetRx::reportChannelLoad()
{
    emit(ChannelLoadSignal, channelLoadSampler.cbr());
}

} // namespace artery
