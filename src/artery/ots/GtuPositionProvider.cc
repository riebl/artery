#include "artery/ots/GtuPositionProvider.h"
#include "artery/ots/GtuInetMobility.h"
#include "artery/networking/Runtime.h"
#include "artery/utility/InitStages.h"
#include <inet/common/ModuleAccess.h>

namespace artery
{

Define_Module(GtuPositionProvider)

using namespace omnetpp;

namespace {
    const simsignal_t positionFixSignal = cComponent::registerSignal("PositionFix");
    const simsignal_t gtuPositionChangedSignal = cComponent::registerSignal("gtuPositionChanged");
} // namespace


void GtuPositionProvider::initialize(int stage)
{
    if (stage == InitStages::Prepare) {
        mRuntime = inet::getModuleFromPar<Runtime>(par("runtimeModule"), this);
        mMobility = inet::getModuleFromPar<GtuInetMobility>(par("mobilityModule"), this);
        auto& mobilityPar = par("mobilityModule");
        auto* mobilityModule = getModuleByPath(mobilityPar);
    } else if (stage == InitStages::Self) {
        mMobility->subscribe(gtuPositionChangedSignal, this);
    } else if (stage == InitStages::Propagate) {
        updatePositionFix();
        omnetpp::createWatch("latitude", mPositionFix.latitude.value());
        omnetpp::createWatch("longitude", mPositionFix.longitude.value());
    }
}

int GtuPositionProvider::numInitStages() const
{
    return InitStages::Total;
}

void GtuPositionProvider::receiveSignal(cComponent*, simsignal_t signal, cObject*, cObject*)
{
    if (signal == gtuPositionChangedSignal) {
        updatePositionFix();
    }
}

void GtuPositionProvider::updatePositionFix()
{
    using namespace vanetza::units;
    static const TrueNorth north;

    auto geopos = mMobility->getGeoPosition();
    mPositionFix.timestamp = mRuntime->now();
    mPositionFix.latitude = geopos.latitude;
    mPositionFix.longitude = geopos.longitude;
    mPositionFix.confidence.semi_minor = 5.0 * si::meter;
    mPositionFix.confidence.semi_major = 5.0 * si::meter;
    mPositionFix.course.assign(
            north + GeoAngle { mMobility->getHeading().getTrueNorth() },
            north + 3.0 * degree);
    mPositionFix.speed.assign(
            mMobility->getLastGtuObject().getSpeed() * si::meter_per_second,
            1.0 * si::meter_per_second);

    // prevent signal listeners to modify our position data
    PositionFixObject tmp { mPositionFix };
    emit(positionFixSignal, &tmp);
}

} // namespace artery
