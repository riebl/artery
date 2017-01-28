#include "artery/nic/RadioDriverBase.h"

using namespace omnetpp;

const simsignal_t RadioDriverBase::ChannelLoadSignal = cComponent::registerSignal("ChannelLoad");

void RadioDriverBase::initialize()
{
    m_middlewareInGate = gate("middleware$i");
    m_middlewareOutGate = gate("middleware$o");
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
    return (msg->getArrivalGate() == m_middlewareInGate);
}

void RadioDriverBase::indicatePacket(cMessage* msg)
{
    send(msg, m_middlewareOutGate);
}
