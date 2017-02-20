#include "traci/Position.h"

namespace traci
{

Position position_cast(const TraCIBoundary& boundary, const TraCIPosition& pos)
{
    const double x = pos.x - boundary.xMin;
    const double y = boundary.yMax - pos.y;
    return Position(x, y);
}

} // namespace traci
