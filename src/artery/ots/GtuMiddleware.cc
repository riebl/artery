#include "artery/ots/GtuMiddleware.h"
#include "artery/ots/GtuInetMobility.h"
#include "artery/utility/InitStages.h"
#include "ots/GtuObject.h"
#include <inet/common/ModuleAccess.h>

namespace artery
{

Define_Module(GtuMiddleware)

using namespace omnetpp;

namespace
{
const simsignal_t gtuPositionChangedSignal = cComponent::registerSignal("gtuPositionChanged");
} // namespace

GtuMiddleware::GtuMiddleware() :
    mVehicleDataProvider(0)
{
}

void GtuMiddleware::initialize(int stage)
{
    if (stage == InitStages::Self) {
        mMobility = inet::getModuleFromPar<GtuInetMobility>(par("mobilityModule"), this);
        mMobility->subscribe(gtuPositionChangedSignal, this);
        initializeStationType(mMobility->getLastGtuObject().getType());
        mVehicleDataProvider.update(getKinematics(*mMobility));
        getFacilities().register_const(&mVehicleDataProvider);
        getFacilities().register_const(&mMobility->getLastGtuObject());

        Identity identity;
        identity.application = Identity::deriveStationId(findHost(), par("stationIdDerivation").stringValue());
        mVehicleDataProvider.setStationId(identity.application);
        // TODO add GTU id to identity?
        emit(Identity::changeSignal, Identity::ChangeStationId, &identity);
    }

    Middleware::initialize(stage);
}

void GtuMiddleware::receiveSignal(cComponent*, simsignal_t signal, cObject*, cObject*)
{
    if (signal == gtuPositionChangedSignal && mMobility) {
        mVehicleDataProvider.update(getKinematics(*mMobility));
    }
}

void GtuMiddleware::initializeStationType(const std::string& gtuType)
{
    using vanetza::geonet::StationType;
    StationType gnStationType = StationType::Unknown;

    if (gtuType == "CAR") {
        gnStationType = StationType::Passenger_Car;
    } else if (gtuType == "EMERGENCY_VEHICLE") {
        gnStationType = StationType::Special_Vehicle;
    } else if (gtuType == "BUS" || gtuType == "SCHEDULED_BUS") {
        gnStationType = StationType::Bus;
    } else if (gtuType == "VAN") {
        gnStationType = StationType::Light_Truck;
    } else if (gtuType == "TRUCK") {
        gnStationType = StationType::Heavy_Truck;
    } else if (gtuType == "BICYCLE") {
        gnStationType = StationType::Cyclist;
    } else if (gtuType == "PEDESTRIAN") {
        gnStationType = StationType::Pedestrian;
    } else if (gtuType == "MOPED") {
        gnStationType = StationType::Moped;
    }

    setStationType(gnStationType);
    mVehicleDataProvider.setStationType(gnStationType);
}

VehicleKinematics GtuMiddleware::getKinematics(const GtuInetMobility& mobility) const
{
    using namespace vanetza::units::si;

    VehicleKinematics kinematics;
    kinematics.position = mobility.getPosition();
    kinematics.geo_position = mobility.getGeoPosition();
    kinematics.heading = mobility.getHeading().getTrueNorth();
    kinematics.speed = mobility.getLastGtuObject().getSpeed() * meter_per_second;
    kinematics.acceleration = mobility.getLastGtuObject().getAcceleration() * meter_per_second_squared;
    return kinematics;
}

} // namespace artery
