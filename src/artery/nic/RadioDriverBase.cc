#include "artery/nic/RadioDriverBase.h"

using namespace omnetpp;

namespace artery
{

const simsignal_t RadioDriverBase::ChannelLoadSignal = cComponent::registerSignal("ChannelLoad");

void RadioDriverBase::initialize()
{
    mUpperLayerIn = gate("upperLayer$i");
    mUpperLayerOut = gate("upperLayer$o");
}

void RadioDriverBase::handleMessage(cMessage* msg)
{
    if (isMiddlewareRequest(msg)) {
        handleUpperMessage(msg);
    } else {
        throw cRuntimeError("unexpected message");
    }
}

bool RadioDriverBase::isMiddlewareRequest(cMessage* msg)
{
    return (msg->getArrivalGate() == mUpperLayerIn);
}

void RadioDriverBase::indicatePacket(cMessage* msg)
{
    send(msg, mUpperLayerOut);
}

} // namespace artery
