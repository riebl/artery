#ifndef ARTERY_GBCMOCKMESSAGE_H_PMHNDBEF
#define ARTERY_GBCMOCKMESSAGE_H_PMHNDBEF

#include "artery/utility/Geometry.h"
#include <omnetpp/cpacket.h>

namespace artery
{

class GbcMockMessage : public omnetpp::cPacket
{
public:
    GbcMockMessage() :
        omnetpp::cPacket("GBC mock message"), mGenerated(omnetpp::simTime()) {}

    void setSourceStation(int id) { mSourceStation = id; }
    int getSourceStation() const { return mSourceStation; }

    void setSourcePosition(const Position& pos) { mSourcePosition = pos; }
    const Position& getSourcePosition() const { return mSourcePosition; }

    omnetpp::SimTime getGenerationTimestamp() const { return mGenerated; }

    omnetpp::cPacket* dup() const override { return new GbcMockMessage(*this); }

private:
    int mSourceStation = 0;
    Position mSourcePosition;
    omnetpp::SimTime mGenerated = omnetpp::SimTime::ZERO;
};

} // namespace artery

#endif /* ARTERY_GBCMOCKMESSAGE_H_PMHNDBEF */

