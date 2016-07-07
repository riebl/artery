#include "artery/inet/InetTraCIMobility.h"
#include "veins/base/utils/FindModule.h"

Define_Module(InetTraCIMobility)

inet::Coord InetTraCIMobility::convert(const Coord& veins_coord) const
{
    return inet::Coord { veins_coord.x, veins_coord.y, veins_coord.z };
}

double InetTraCIMobility::getMaxSpeed() const
{
    return NaN;
}

inet::Coord InetTraCIMobility::getCurrentPosition()
{
    return convert(veins_base::getCurrentPosition());
}

inet::Coord InetTraCIMobility::getCurrentSpeed()
{
    return convert(veins_base::getCurrentSpeed());
}

inet::EulerAngles InetTraCIMobility::getCurrentAngularPosition()
{
    return inet::EulerAngles::ZERO;
}

inet::EulerAngles InetTraCIMobility::getCurrentAngularSpeed()
{
    return inet::EulerAngles::ZERO;
}

inet::Coord InetTraCIMobility::getConstraintAreaMax() const
{
    return inet::Coord {
	veins_base::playgroundSizeX(),
	veins_base::playgroundSizeY(),
	veins_base::playgroundSizeZ()
    };
}

inet::Coord InetTraCIMobility::getConstraintAreaMin() const
{
    return inet::Coord::ZERO;
}

void InetTraCIMobility::updatePosition()
{
    veins_base::updatePosition();
    emit(inet_base::mobilityStateChangedSignal, this);
}

Veins::TraCIScenarioManagerBase* InetTraCIMobility::getManager() const
{
    return FindModule<Veins::TraCIScenarioManagerBase*>::findGlobalModule();
}
