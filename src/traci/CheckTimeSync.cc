#include "traci/CheckTimeSync.h"
#include "traci/VariableCache.h"
#include "traci/sumo/libsumo/TraCIConstants.h"
#include <cmath>

namespace traci
{

bool checkTimeSync(SimulationCache& cache, omnetpp::SimTime time)
{
    const double deviation = cache.get<libsumo::VAR_TIME>() - time.dbl();
    const double threshold = 0.01 * cache.get<libsumo::VAR_DELTA_T>();
    return std::abs(deviation) < threshold;
}

} // namespace traci
