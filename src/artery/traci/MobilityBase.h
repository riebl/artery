#ifndef ARTERY_MOBILITYBASE_H_1SQMAVHF
#define ARTERY_MOBILITYBASE_H_1SQMAVHF

#include "artery/traci/ControllableVehicle.h"
#include "traci/API.h"
#include "traci/VehicleSink.h"
#include "traci/VariableCache.h"
#include <omnetpp/clistener.h>
#include <memory>
#include <string>

namespace artery
{

class MobilityBase :
    public traci::VehicleSink, // for receiving updates from TraCI
    public ControllableVehicle // for controlling the vehicle via TraCI
{
public:
    // traci::VehicleSink interface
    void initializeSink(std::shared_ptr<traci::API>, std::shared_ptr<traci::VehicleCache>, const traci::Boundary&) override;
    void initializeVehicle(const traci::TraCIPosition&, traci::TraCIAngle, double speed) override;
    void updateVehicle(const traci::TraCIPosition&, traci::TraCIAngle, double speed) override;

    // ControllableVehicle
    traci::VehicleController* getVehicleController() override;

    // generic signal for mobility state changes
    static omnetpp::simsignal_t stateChangedSignal;

protected:
    std::string mVehicleId;
    std::shared_ptr<traci::API> mTraci;
    traci::Boundary mNetBoundary;

private:
    virtual void initialize(const Position&, Angle, double speed) = 0;
    virtual void update(const Position&, Angle, double speed) = 0;

    std::unique_ptr<traci::VehicleController> mController;
};

} // namespace artery

#endif /* ARTERY_MOBILITYBASE_H_1SQMAVHF */
