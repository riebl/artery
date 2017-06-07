#include "artery/inet/InetMobility.h"
#include "artery/traci/VehicleController.h"
#include <inet/common/ModuleAccess.h>
#include <inet/common/geometry/common/CanvasProjection.h>
#include <inet/visualizer/mobility/MobilityCanvasVisualizer.h>
#include <cmath>

Define_Module(InetMobility)


int InetMobility::numInitStages() const
{
    return inet::INITSTAGE_PHYSICAL_ENVIRONMENT_2 + 1;
}

void InetMobility::initialize(int stage)
{
    if (stage == inet::INITSTAGE_LOCAL) {
        mVisualRepresentation = inet::getModuleFromPar<cModule>(par("visualRepresentation"), this, false);
        mAntennaHeight = par("antennaHeight");
        WATCH(mVehicleId);
        WATCH(mPosition);
        WATCH(mSpeed);
        WATCH(mOrientation);
    } else if (stage == inet::INITSTAGE_PHYSICAL_ENVIRONMENT_2) {
        if (mVisualRepresentation) {
            auto visualizationTarget = mVisualRepresentation->getParentModule();
            mCanvasProjection = inet::CanvasProjection::getCanvasProjection(visualizationTarget->getCanvas());
        }
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
    return inet::Coord { mNetBoundary.xMax, mNetBoundary.yMax, mNetBoundary.zMax };
}

inet::Coord InetMobility::getConstraintAreaMin() const
{
    return inet::Coord { mNetBoundary.xMin, mNetBoundary.yMin, mNetBoundary.zMin };
}

void InetMobility::update(const Position& pos, Angle heading, double speed)
{
    using boost::units::si::meter;
    const double rad = heading.radian();
    const inet::Coord direction { cos(rad), -sin(rad) };
    mPosition = inet::Coord { pos.x / meter, pos.y / meter, mAntennaHeight };
    mSpeed = direction * speed;
    mOrientation.alpha = -rad;

    emit(inet::IMobility::mobilityStateChangedSignal, this);
    updateVisualRepresentation();
}

void InetMobility::updateVisualRepresentation()
{
    if (hasGUI() && mVisualRepresentation) {
        using inet::visualizer::MobilityCanvasVisualizer;
        MobilityCanvasVisualizer::setPosition(mVisualRepresentation, mCanvasProjection->computeCanvasPoint(getCurrentPosition()));
    }
}
