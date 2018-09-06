#ifndef CHECKTIMESYNC_H_XTJESFQX
#define CHECKTIMESYNC_H_XTJESFQX

#include <omnetpp/simtime.h>

namespace traci
{

class SimulationCache;

/**
 * Check if TraCI simulation cache's time is synchronised with OMNeT++'s time
 * \param cache simulation variable cache
 * \param time current simulation time
 * \return true if in sync
 */
bool checkTimeSync(SimulationCache& cache, omnetpp::SimTime time);

} // namespace traci

#endif /* CHECKTIMESYNC_H_XTJESFQX */

