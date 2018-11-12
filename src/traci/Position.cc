#include "traci/Position.h"

namespace traci
{

artery::Position position_cast(const TraCIBoundary& boundary, const TraCIPosition& pos)
{
    const double x = pos.x - boundary.xMin;
    const double y = boundary.yMax - pos.y;
    return artery::Position(x, y);
}

TraCIPosition position_cast(const TraCIBoundary& boundary, const artery::Position& pos)
{
    const double x = pos.x.value() + boundary.xMin;
    const double y = boundary.yMax - pos.y.value();
    return TraCIPosition { x, y, 0.0 };
}

} // namespace traci
