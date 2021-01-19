#ifndef ARTERY_TRACI_H_E0WJI6AS
#define ARTERY_TRACI_H_E0WJI6AS

#include "artery/utility/Geometry.h"
#include "traci/Angle.h"
#include "traci/Boundary.h"
#include "traci/Position.h"

namespace artery
{

Angle angle_cast(traci::TraCIAngle);
traci::TraCIAngle angle_cast(Angle);

Position position_cast(const traci::Boundary&, const traci::TraCIPosition&);
traci::TraCIPosition position_cast(const traci::Boundary&, const artery::Position&);

} // namespace artery


namespace traci
{

using artery::angle_cast;
using artery::position_cast;

} // namespace traci

#endif /* ARTERY_TRACI_H_E0WJI6AS */

