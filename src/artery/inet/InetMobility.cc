#include "artery/inet/InetMobility.h"
#include "artery/traci/VehicleController.h"
#include <inet/common/ModuleAccess.h>
#include <inet/common/geometry/common/CanvasProjection.h>
#include <inet/features.h>
#include <cmath>

#ifdef WITH_VISUALIZERS
#   include <inet/visualizer/mobility/MobilityCanvasVisualizer.h>
#else
#   include <cstdio>
#endif

namespace artery
{

Define_Module(InetPersonMobility)
Define_Module(InetVehicleMobility)


int InetMobility::numInitStages() const
{
    return inet::INITSTAGE_PHYSICAL_ENVIRONMENT_2 + 1;
}

void InetMobility::initialize(int stage)
{
    if (stage == inet::INITSTAGE_LOCAL) {
        mVisualRepresentation = inet::getModuleFromPar<cModule>(par("visualRepresentation"), this, false);
        mAntennaHeight = par("antennaHeight");
        WATCH(mPosition);
        WATCH(mSpeed);
        WATCH(mOrientation);
    } else if (stage == inet::INITSTAGE_PHYSICAL_ENVIRONMENT_2) {
        if (mVisualRepresentation) {
            auto visualizationTarget = mVisualRepresentation->getParentModule();
            mCanvasProjection = inet::CanvasProjection::getCanvasProjection(visualizationTarget->getCanvas());
        }
        emit(MobilityBase::stateChangedSignal, this);
        updateVisualRepresentation();
    }
}

double InetMobility::getMaxSpeed() const
{
    return NaN;
}

inet::Coord InetMobility::getCurrentPosition()
{
    return mPosition;
}

inet::Coord InetMobility::getCurrentSpeed()
{
    return mSpeed;
}

inet::EulerAngles InetMobility::getCurrentAngularPosition()
{
    return mOrientation;
}

inet::EulerAngles InetMobility::getCurrentAngularSpeed()
{
    return inet::EulerAngles::ZERO;
}

inet::Coord InetMobility::getConstraintAreaMax() const
{
    const auto& max = mNetBoundary.upperRightPosition();
    return inet::Coord { max.x, max.y, max.z };
}

inet::Coord InetMobility::getConstraintAreaMin() const
{
    const auto& min = mNetBoundary.lowerLeftPosition();
    return inet::Coord { min.x, min.y, min.z };
}

void InetMobility::initialize(const Position& pos, Angle heading, double speed)
{
    using boost::units::si::meter;
    const double rad = heading.radian();
    const inet::Coord direction { cos(rad), -sin(rad) };
    mPosition = inet::Coord { pos.x / meter, pos.y / meter, mAntennaHeight };
    mSpeed = direction * speed;
    mOrientation.alpha = -rad;
}

void InetMobility::update(const Position& pos, Angle heading, double speed)
{
    initialize(pos, heading, speed);
    ASSERT(inet::IMobility::mobilityStateChangedSignal == MobilityBase::stateChangedSignal);
    emit(MobilityBase::stateChangedSignal, this);
    updateVisualRepresentation();
}

void InetMobility::updateVisualRepresentation()
{
    // following code is taken from INET's MobilityBase::updateVisualRepresentation
    if (hasGUI() && mVisualRepresentation) {
#ifdef WITH_VISUALIZERS
        using inet::visualizer::MobilityCanvasVisualizer;
        MobilityCanvasVisualizer::setPosition(mVisualRepresentation, mCanvasProjection->computeCanvasPoint(getCurrentPosition()));
#else
        auto position = mCanvasProjection->computeCanvasPoint(getCurrentPosition());
        char buf[32];
        snprintf(buf, sizeof(buf), "%lf", position.x);
        buf[sizeof(buf) - 1] = 0;
        mVisualRepresentation->getDisplayString().setTagArg("p", 0, buf);
        snprintf(buf, sizeof(buf), "%lf", position.y);
        buf[sizeof(buf) - 1] = 0;
        mVisualRepresentation->getDisplayString().setTagArg("p", 1, buf);
#endif
    }
}

void InetPersonMobility::initialize(int stage)
{
    if (stage == 0) {
        WATCH(mPersonId);
    }

    InetMobility::initialize(stage);
}

void InetVehicleMobility::initialize(int stage)
{
    if (stage == 0) {
        WATCH(mVehicleId);
    }

    InetMobility::initialize(stage);
}

} // namespace artery
