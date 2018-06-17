#ifndef BLACKICEWARNERD2D_H_HH5TITG4
#define BLACKICEWARNERD2D_H_HH5TITG4

#include <inet/transportlayer/contract/udp/UDPSocket.h>
#include <omnetpp/clistener.h>
#include <omnetpp/csimplemodule.h>

// forward declaration
class BlackIceReport;
namespace traci { class VehicleController; }

class BlackIceWarnerD2D : public omnetpp::cSimpleModule, public omnetpp::cListener
{
public:
    ~BlackIceWarnerD2D();

protected:
    int numInitStages() const override;
    void initialize(int stage) override;
    void finish() override;
    void handleMessage(omnetpp::cMessage*) override;
    void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t, omnetpp::cObject*, omnetpp::cObject*) override;

private:
    void sendReport();
    void processReport(BlackIceReport&);

    inet::UDPSocket socket;
    inet::L3Address mcastAddress;
    int mcastPort;
    int numWarningsPeer;
    int tractionLosses;
    double warningRadius;
    double warningDuration;
    traci::VehicleController* vehicleController = nullptr;
    omnetpp::cMessage* removeSpeedReduction;
};


#endif /* BLACKICEWARNERD2D_H_HH5TITG4 */

