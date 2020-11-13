#ifndef ARTERY_RTCMMOCKMESSAGE_H_A60J9GBE
#define ARTERY_RTCMMOCKMESSAGE_H_A60J9GBE

#include "artery/utility/Geometry.h"
#include <omnetpp/cpacket.h>

namespace artery
{

class RtcmMockMessage : public omnetpp::cPacket
{
public:
    RtcmMockMessage();

    void setSourceStation(int id) { mSourceStation = id; }
    int getSourceStation() const { return mSourceStation; }

    void setSourcePosition(const Position& pos) { mSourcePosition = pos; }
    const Position& getSourcePosition() const { return mSourcePosition; }

    omnetpp::SimTime getGenerationTimestamp() const { return mGenerated; }

    omnetpp::cPacket* dup() const override;

private:
    int mSourceStation = 0;
    Position mSourcePosition;
    omnetpp::SimTime mGenerated = omnetpp::SimTime::ZERO;
};

} // namespace artery

#endif /* ARTERY_RTCMMOCKMESSAGE_H_A60J9GBE */

