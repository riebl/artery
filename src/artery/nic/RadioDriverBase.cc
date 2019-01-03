#include "artery/nic/RadioDriverBase.h"
#include "artery/nic/RadioDriverProperties.h"

using namespace omnetpp;

namespace artery
{

const simsignal_t RadioDriverBase::ChannelLoadSignal = cComponent::registerSignal("ChannelLoad");

void RadioDriverBase::initialize()
{
    mUpperLayerIn = gate("upperLayer$i");
    mUpperLayerOut = gate("upperLayer$o");
    mPropertiesOut = gate("properties");
}

void RadioDriverBase::handleMessage(cMessage* msg)
{
    if (isDataRequest(msg)) {
        handleDataRequest(msg);
    } else {
        throw cRuntimeError("unexpected message");
    }
}

bool RadioDriverBase::isDataRequest(cMessage* msg)
{
    return (msg->getArrivalGate() == mUpperLayerIn);
}

void RadioDriverBase::indicateData(cMessage* msg)
{
    send(msg, mUpperLayerOut);
}

void RadioDriverBase::indicateProperties(RadioDriverProperties* properties)
{
    send(properties, mPropertiesOut);
}

} // namespace artery
