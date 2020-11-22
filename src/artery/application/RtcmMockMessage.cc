#include "artery/application/RtcmMockMessage.h"
#include <omnetpp.h>

using namespace omnetpp;

namespace artery
{

Register_Class(RtcmMockMessage)

RtcmMockMessage::RtcmMockMessage() :
    cPacket("RTCM mock message"),
    mGenerated(omnetpp::simTime())
{
}

cPacket* RtcmMockMessage::dup() const
{
    return new RtcmMockMessage(*this);
}


class RtcmSourceResultFilter : public cObjectResultFilter
{
protected:
    void receiveSignal(cResultFilter* prev, simtime_t_cref t, cObject* object, cObject* details) override
    {
        if (auto rtcm = dynamic_cast<RtcmMockMessage*>(object)) {
            fire(this, t, static_cast<long>(rtcm->getSourceStation()), details);
        }
    }
};

Register_ResultFilter("rtcmSource", RtcmSourceResultFilter)


class RtcmGeneratedResultFilter : public cObjectResultFilter
{
protected:
    void receiveSignal(cResultFilter* prev, simtime_t_cref t, cObject* object, cObject* details) override
    {
        if (auto rtcm = dynamic_cast<RtcmMockMessage*>(object)) {
            fire(this, t, rtcm->getCreationTime(), details);
        }
    }
};

Register_ResultFilter("rtcmGenerated", RtcmGeneratedResultFilter)

} // namespace artery
