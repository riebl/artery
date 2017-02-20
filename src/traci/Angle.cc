#include "traci/Angle.h"
#include <boost/math/constants/constants.hpp>
#include <boost/units/systems/si/plane_angle.hpp>

using boost::math::double_constants::pi;

namespace traci
{

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

    return TraCIAngle(traci);
}

} // namespace traci
