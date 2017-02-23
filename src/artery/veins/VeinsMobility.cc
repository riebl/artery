#include "artery/veins/VeinsMobility.h"
#include <cmath>

Define_Module(VeinsMobility)

namespace
{
    const auto mobilityStateChangedSignal = omnetpp::cComponent::registerSignal("mobilityStateChanged");
}

void VeinsMobility::initialize(int stage)
{
    BaseMobility::initialize(stage);
    if (stage == 0) {
        mPosition.z = par("z");
        WATCH(mVehicleId);
        WATCH(mPosition);
        WATCH(mDirection);
        WATCH(mSpeed);
    }
}

void VeinsMobility::update(const Position& pos, Angle heading, double speed)
{
    using boost::units::si::meter;
    mPosition.x = pos.x / meter;
    mPosition.y = pos.y / meter;
    move.setStart(mPosition);

    mSpeed = speed;
    move.setSpeed(mSpeed);

    mDirection = Coord { cos(heading.radian()), -sin(heading.radian()) };
    move.setDirectionByVector(mDirection);

    BaseMobility::updatePosition(); // emits update signal for Veins
    emit(mobilityStateChangedSignal, this);
}
