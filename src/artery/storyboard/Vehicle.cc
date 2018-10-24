#include "artery/storyboard/Vehicle.h"
#include "artery/application/StoryboardSignal.h"
#include <omnetpp/ccomponent.h>

using namespace omnetpp;

namespace artery
{

static const simsignal_t signalStoryboard = cComponent::registerSignal("StoryboardSignal");

Vehicle::Vehicle(artery::Middleware& mw, std::map<std::string, Vehicle>& vs) :
    mMiddleware(mw), mVehicles(vs)
{
}

const std::string& Vehicle::getId() const
{
    return getController().getVehicleId();
}

traci::VehicleController& Vehicle::getController()
{
    return mMiddleware.getFacilities().get_mutable<traci::VehicleController>();
}

const traci::VehicleController& Vehicle::getController() const
{
    return mMiddleware.getFacilities().get_const<traci::VehicleController>();
}

const std::map<std::string, Vehicle>& Vehicle::getVehicles() const
{
    return mVehicles;
}

void Vehicle::emit(const StoryboardSignal& signal) const
{
    const omnetpp::cObject* obj = &signal;
    mMiddleware.emit(signalStoryboard, obj);
}

} // namespace artery
