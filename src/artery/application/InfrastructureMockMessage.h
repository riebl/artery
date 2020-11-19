#ifndef ARTERY_INFRASTRUCTUREMOCKMESSAGE_H_AEPKF5GQ
#define ARTERY_INFRASTRUCTUREMOCKMESSAGE_H_AEPKF5GQ

#include <omnetpp/cpacket.h>

namespace artery
{

class InfrastructureMockMessage : public omnetpp::cPacket
{
public:
    InfrastructureMockMessage();
    omnetpp::cPacket* dup() const override;

    void setSourceStation(int id) { mSourceStation = id; }
    int getSourceStation() const { return mSourceStation; }

    void setSequenceNumber(int n) { mSequenceNumber = n; }
    int getSequenceNumber() const { return mSequenceNumber; }

    omnetpp::SimTime getGenerationTimestamp() const { return mGenerated; }

private:
    int mSourceStation = 0;
    int mSequenceNumber = 0;
    omnetpp::SimTime mGenerated = omnetpp::SimTime::ZERO;
};

} // namespace artery

#endif /* ARTERY_INFRASTRUCTUREMOCKMESSAGE_H_AEPKF5GQ */

