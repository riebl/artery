#include "artery/traci/Cast.h"
#include <boost/math/constants/constants.hpp>
#include <boost/units/systems/si/plane_angle.hpp>

namespace artery
{

using boost::math::double_constants::pi;
using traci::TraCIAngle;
using traci::TraCIPosition;

Angle angle_cast(TraCIAngle traci)
{
    // change orientation and align "neutral" angle to east
    double opp = 90.0 - traci.degree;
    // convert to radian
    opp *= pi / 180.0;

    return Angle(opp * boost::units::si::radian);
}

TraCIAngle angle_cast(Angle opp)
{
    // change orientation and align "neutral" angle to north
    double traci = 0.5 * pi - opp.radian();
    // convert to degree
    traci *= 180.0 / pi;

    return traci::TraCIAngle(traci);
}

Position position_cast(const traci::Boundary& boundary, const TraCIPosition& pos)
{
    const double x = pos.x - boundary.lowerLeftPosition().x;
    const double y = boundary.upperRightPosition().y - pos.y;
    return Position(x, y);
}

TraCIPosition position_cast(const traci::Boundary& boundary, const Position& pos)
{
    const double x = pos.x.value() + boundary.lowerLeftPosition().x;
    const double y = boundary.upperRightPosition().y - pos.y.value();
    TraCIPosition tmp;
    tmp.x = x;
    tmp.y = y;
    tmp.z = 0.0;
    return tmp;
}

} // namespace artery
