#include "artery/application/InfrastructureMockMessage.h"
#include <omnetpp.h>

using namespace omnetpp;

namespace artery
{

Register_Class(InfrastructureMockMessage)

InfrastructureMockMessage::InfrastructureMockMessage() :
    cPacket("infrastructure mock message"),
    mGenerated(omnetpp::simTime())
{
}

cPacket* InfrastructureMockMessage::dup() const
{
    return new InfrastructureMockMessage(*this);
}


class InfrastructureSourceResultFilter : public cObjectResultFilter
{
protected:
    void receiveSignal(cResultFilter* prev, simtime_t_cref t, cObject* object, cObject* details) override
    {
        if (auto msg = dynamic_cast<InfrastructureMockMessage*>(object)) {
            fire(this, t, static_cast<long>(msg->getSourceStation()), details);
        }
    }
};

Register_ResultFilter("immSource", InfrastructureSourceResultFilter)


class InfrastructureSequenceNumberResultFilter : public cObjectResultFilter
{
protected:
    void receiveSignal(cResultFilter* prev, simtime_t_cref t, cObject* object, cObject* details) override
    {
        if (auto msg = dynamic_cast<InfrastructureMockMessage*>(object)) {
            fire(this, t, static_cast<long>(msg->getSequenceNumber()), details);
        }
    }
};

Register_ResultFilter("immSequenceNumber", InfrastructureSequenceNumberResultFilter)

} // namespace artery
