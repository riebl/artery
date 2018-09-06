#include "traci/Position.h"

namespace traci
{

artery::Position position_cast(const Boundary& boundary, const TraCIPosition& pos)
{
    const double x = pos.x - boundary.lowerLeftPosition().x;
    const double y = boundary.upperRightPosition().y - pos.y;
    return artery::Position(x, y);
}

TraCIPosition position_cast(const Boundary& boundary, const artery::Position& pos)
{
    const double x = pos.x.value() + boundary.lowerLeftPosition().x;
    const double y = boundary.upperRightPosition().y - pos.y.value();
    TraCIPosition tmp;
    tmp.x = x;
    tmp.y = y;
    tmp.z = 0.0;
    return tmp;
}

} // namespace traci
