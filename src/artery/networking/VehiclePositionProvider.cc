#include "artery/networking/Runtime.h"
#include "artery/networking/VehiclePositionProvider.h"
#include "artery/traci/MobilityBase.h"
#include "artery/utility/InitStages.h"
#include "inet/common/ModuleAccess.h"

namespace artery
{

Define_Module(VehiclePositionProvider)

static const omnetpp::simsignal_t scPositionFixSignal = omnetpp::cComponent::registerSignal("PositionFix");

void VehiclePositionProvider::initialize(int stage)
{
    if (stage == InitStages::Prepare) {
        mRuntime = inet::getModuleFromPar<Runtime>(par("runtimeModule"), this);
        auto& mobilityPar = par("mobilityModule");
        auto* mobilityModule = getModuleByPath(mobilityPar);
        if (mobilityModule) {
            mobilityModule->subscribe(MobilityBase::stateChangedSignal, this);
            if (auto mobilityBase = dynamic_cast<MobilityBase*>(mobilityModule)) {
                mVehicleController = mobilityBase->getVehicleController();
            } else {
                error("Module on path '%s' is not a MobilityBase", mobilityModule->getFullPath().c_str());
            }
        } else {
            error("Module not found on path '%s' defined by par '%s'",
                    mobilityPar.stringValue(), mobilityPar.getFullPath().c_str());
        }
    } else if (stage == InitStages::Propagate) {
        updatePosition();
    }
}

int VehiclePositionProvider::numInitStages() const
{
    return InitStages::Total;
}

void VehiclePositionProvider::receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t signal, omnetpp::cObject*, omnetpp::cObject*)
{
    if (signal == MobilityBase::stateChangedSignal && mVehicleController) {
        updatePosition();
    }
}

void VehiclePositionProvider::updatePosition()
{
    using namespace vanetza::units;
    static const TrueNorth north;

    auto geopos = mVehicleController->getGeoPosition();
    mPositionFix.timestamp = mRuntime->now();
    mPositionFix.latitude = geopos.latitude;
    mPositionFix.longitude = geopos.longitude;
    mPositionFix.confidence.semi_minor = 5.0 * si::meter;
    mPositionFix.confidence.semi_major = 5.0 * si::meter;
    mPositionFix.course.assign(north + GeoAngle { mVehicleController->getHeading().getTrueNorth() }, north + 3.0 * degree);
    mPositionFix.speed.assign(mVehicleController->getSpeed(), 1.0 * si::meter_per_second);

    // prevent signal listeners to modify our position data
    PositionFixObject tmp { mPositionFix };
    emit(scPositionFixSignal, &tmp);
}

} // namespace artery
