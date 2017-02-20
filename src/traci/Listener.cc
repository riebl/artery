#include "traci/Listener.h"
#include <omnetpp/ccomponent.h>

using namespace omnetpp;

namespace traci
{

namespace
{

const simsignal_t initSignal = cComponent::registerSignal("traci.init");
const simsignal_t stepSignal = cComponent::registerSignal("traci.step");
const simsignal_t closeSignal = cComponent::registerSignal("traci.close");

}

Listener::Listener() : m_publisher(nullptr)
{
}

void Listener::subscribeTraCI(cComponent* publisher)
{
    ASSERT(publisher);

    unsubscribeTraCI();
    m_publisher = publisher;
    m_publisher->subscribe(initSignal, this);
    m_publisher->subscribe(stepSignal, this);
    m_publisher->subscribe(closeSignal, this);
}

void Listener::unsubscribeTraCI()
{
    if (m_publisher) {
        m_publisher->unsubscribe(initSignal, this);
        m_publisher->unsubscribe(stepSignal, this);
        m_publisher->unsubscribe(closeSignal, this);
    }
}

void Listener::receiveSignal(cComponent*, simsignal_t signal, const SimTime&, cObject*)
{
    if (signal == stepSignal) {
        traciStep();
    } else if (signal == initSignal) {
        traciInit();
    } else if (signal == closeSignal) {
        traciClose();
    }
}

void Listener::traciInit()
{
}

void Listener::traciStep()
{
}

void Listener::traciClose()
{
}

} // namespace traci
