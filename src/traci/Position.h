#ifndef POSITION_H_ECOTFZI7
#define POSITION_H_ECOTFZI7

#include "artery/utility/Geometry.h"
#include "traci/Boundary.h"
#include "traci/sumo/utils/traci/TraCIAPI.h"

namespace traci
{

using TraCIPosition = TraCIPosition;

Position position_cast(const TraCIBoundary&, const TraCIPosition&);

} // namespace traci

#endif /* POSITION_H_ECOTFZI7 */

