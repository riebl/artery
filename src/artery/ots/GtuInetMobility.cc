#include "artery/ots/GtuInetMobility.h"
#include <inet/common/ModuleAccess.h>
#include <inet/common/geometry/common/CanvasProjection.h>
#include <inet/common/geometry/common/CoordinateSystem.h>
#include <inet/features.h>
#include <cmath>

#ifdef WITH_VISUALIZERS
#   include <inet/visualizer/mobility/MobilityCanvasVisualizer.h>
#else
#   include <cstdio>
#endif

namespace artery
{

Define_Module(GtuInetMobility)

namespace
{

using namespace omnetpp;
const simsignal_t gtuPositionChangedSignal = cComponent::registerSignal("gtuPositionChanged");

} // namespace

int GtuInetMobility::numInitStages() const
{
    return inet::INITSTAGE_PHYSICAL_ENVIRONMENT_2 + 1;
}

void GtuInetMobility::initialize(int stage)
{
    if (stage == inet::INITSTAGE_LOCAL) {
        mVisualRepresentation = inet::getModuleFromPar<cModule>(par("visualRepresentation"), this, false);
        mCoordinateSystem = inet::getModuleFromPar<inet::IGeographicCoordinateSystem>(par("coordinateSystemModule"), this);
        omnetpp::createWatch("position", mPosition);
        omnetpp::createWatch("speed", mSpeed);
        omnetpp::createWatch("orientation", mOrientation);
        omnetpp::createWatch("GTU ID", mLastGtuObject.getId());
    } else if (stage == inet::INITSTAGE_PHYSICAL_ENVIRONMENT_2) {
        if (mVisualRepresentation) {
            auto target = mVisualRepresentation->getParentModule();
            mCanvasProjection = inet::CanvasProjection::getCanvasProjection(target->getCanvas());
        }
        updateVisualRepresentation();
    }
}

double GtuInetMobility::getMaxSpeed() const
{
    return NaN;
}

inet::Coord GtuInetMobility::getCurrentPosition()
{
    return mPosition;
}

inet::Coord GtuInetMobility::getCurrentSpeed()
{
    return mSpeed;
}

inet::EulerAngles GtuInetMobility::getCurrentAngularPosition()
{
    return mOrientation;
}

inet::EulerAngles GtuInetMobility::getCurrentAngularSpeed()
{
    return inet::EulerAngles::ZERO;
}

inet::Coord GtuInetMobility::getConstraintAreaMax() const
{
    return inet::Coord::NIL;
}

inet::Coord GtuInetMobility::getConstraintAreaMin() const
{
    return inet::Coord::NIL;
}

void GtuInetMobility::initialize(const ots::GtuObject& gtu)
{
    setInetProperties(gtu);
    mLastGtuObject = gtu;
}

void GtuInetMobility::update(const ots::GtuObject& gtu)
{
    setInetProperties(gtu);
    emit(inet::IMobility::mobilityStateChangedSignal, this);
    emit(gtuPositionChangedSignal, &gtu);
    updateVisualRepresentation();
    mLastGtuObject = gtu;
}

void GtuInetMobility::setInetProperties(const ots::GtuObject& gtu)
{
    const double headingRad = gtu.getHeadingRad();
    const inet::Coord direction { std::cos(headingRad), -std::sin(headingRad) };
    mOrientation.alpha = -headingRad;
    mSpeed = direction * gtu.getSpeed();
    std::tie(mPosition.x, mPosition.y, mPosition.z) = gtu.getPosition();
    mPosition.y *= -1.0; // flix y-axis (positive is up in OTS vs. negative is up in OMNeT++/INET)
}

void GtuInetMobility::updateVisualRepresentation()
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

Position GtuInetMobility::getPosition() const
{
    return Position { mPosition.x, mPosition.y };
}

GeoPosition GtuInetMobility::getGeoPosition() const
{
    inet::GeoCoord inet = mCoordinateSystem->computeGeographicCoordinate(mPosition);
    GeoPosition pos;
    pos.latitude = inet.latitude * boost::units::degree::degree;
    pos.longitude = inet.longitude * boost::units::degree::degree;
    return pos;
}

Angle GtuInetMobility::getHeading() const
{
    return Angle { mLastGtuObject.getHeadingRad() };
}

} // namespace artery
