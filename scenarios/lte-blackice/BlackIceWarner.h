#ifndef BLACKICEWARNER_H_IONPZSRI
#define BLACKICEWARNER_H_IONPZSRI

#include <inet/transportlayer/contract/udp/UDPSocket.h>
#include <omnetpp/csimplemodule.h>

// forward declaration
class BlackIceResponse;
namespace traci { class VehicleController; }

class BlackIceWarner : public omnetpp::cSimpleModule
{
public:
    ~BlackIceWarner();

protected:
    int numInitStages() const override;
    void initialize(int stage) override;
    void finish() override;
    void handleMessage(omnetpp::cMessage*) override;

private:
    void pollCentral();
    void processResponse(BlackIceResponse&);

    inet::UDPSocket socket;
    double pollingRadius;
    omnetpp::simtime_t pollingInterval;
    omnetpp::cMessage* pollingTrigger = nullptr;
    traci::VehicleController* vehicleController = nullptr;
    bool reducedSpeed = false;
    int numWarningsCentral;
};

#endif /* BLACKICEWARNER_H_IONPZSRI */

