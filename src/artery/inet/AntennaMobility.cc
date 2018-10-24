#include <artery/inet/AntennaMobility.h>
#include <omnetpp/cexception.h>

namespace artery
{

Define_Module(AntennaMobility)

void AntennaMobility::initialize(int stage)
{
    omnetpp::cModule* module = getModuleByPath(par("mobilityModule"));
    mParentMobility = check_and_cast<inet::IMobility*>(module);

    mOffsetCoord.x = par("offsetX");
    mOffsetCoord.y = par("offsetY");
    mOffsetCoord.z = par("offsetZ");
    mOffsetAngles.alpha = par("offsetAlpha");
    mOffsetAngles.beta = par("offsetBeta");
    mOffsetAngles.gamma = par("offsetGamma");
    mOffsetRotation = inet::Rotation(mOffsetAngles);
}

int AntennaMobility::numInitStages() const
{
    return 1;
}

double AntennaMobility::getMaxSpeed() const
{
    return mParentMobility->getMaxSpeed();
}

inet::Coord AntennaMobility::getCurrentPosition()
{
    inet::EulerAngles angular_pos = mParentMobility->getCurrentAngularPosition();
    std::swap(angular_pos.alpha, angular_pos.gamma);
    inet::Rotation rot(angular_pos);
    inet::Coord rotated_offset = rot.rotateVectorClockwise(mOffsetCoord);
    return mParentMobility->getCurrentPosition() + rotated_offset;
}

inet::Coord AntennaMobility::getCurrentSpeed()
{
    return mOffsetRotation.rotateVectorClockwise(mParentMobility->getCurrentSpeed());
}

inet::EulerAngles AntennaMobility::getCurrentAngularPosition()
{
    return mParentMobility->getCurrentAngularPosition() + mOffsetAngles;
}

inet::EulerAngles AntennaMobility::getCurrentAngularSpeed()
{
    inet::EulerAngles speed = mParentMobility->getCurrentAngularSpeed();
    if (speed.alpha != 0.0 || speed.beta != 0.0 || speed.gamma != 0.0) {
        throw omnetpp::cRuntimeError("non-zero angular speed is not supported");
    }

    return inet::EulerAngles::ZERO;
}

inet::Coord AntennaMobility::getConstraintAreaMax() const
{
    inet::Coord offset = mParentMobility->getConstraintAreaMax() + mOffsetCoord;
    return offset.max(mParentMobility->getConstraintAreaMax());
}

inet::Coord AntennaMobility::getConstraintAreaMin() const
{
    inet::Coord offset = mParentMobility->getConstraintAreaMin() + mOffsetCoord;
    return offset.min(mParentMobility->getConstraintAreaMin());
}

} // namespace artery
