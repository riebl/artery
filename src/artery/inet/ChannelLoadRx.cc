#include "artery/inet/ChannelLoadRx.h"

using namespace omnetpp;

namespace artery
{

const simsignal_t ChannelLoadRx::ChannelLoadSignal = cComponent::registerSignal("ChannelLoad");

Define_Module(ChannelLoadRx)

ChannelLoadRx::ChannelLoadRx()
{
}

ChannelLoadRx::~ChannelLoadRx()
{
    cancelAndDelete(mChannelReportTrigger);
}

void ChannelLoadRx::initialize(int stage)
{
    Rx::initialize(stage);
    if (stage == 0) {
        mCbrWithTx = par("cbrWithTx");
        mChannelReportInterval = simtime_t { 100, SIMTIME_MS };
        mChannelReportTrigger = new cMessage("report CL");
        scheduleAt(simTime() + mChannelReportInterval, mChannelReportTrigger);

        omnetpp::createWatch("channelLoadSampler", mChannelLoadSampler);
    }
}

void ChannelLoadRx::handleMessage(cMessage* msg)
{
    if (msg == mChannelReportTrigger) {
        emit(ChannelLoadSignal, mChannelLoadSampler.cbr());
        scheduleAt(simTime() + mChannelReportInterval, mChannelReportTrigger);
    } else {
        Rx::handleMessage(msg);
    }
}

void ChannelLoadRx::recomputeMediumFree()
{
    Rx::recomputeMediumFree();

    using ReceptionState = inet::physicallayer::IRadio::ReceptionState;
    if (mCbrWithTx) {
        mChannelLoadSampler.busy(!mediumFree);
    } else {
        mChannelLoadSampler.busy(!mediumFree && receptionState > ReceptionState::RECEPTION_STATE_IDLE);
    }
}

} // namespace artery
