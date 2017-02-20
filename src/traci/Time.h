#ifndef TIME_H_TRBXBVQM
#define TIME_H_TRBXBVQM

#include "traci/sumo/utils/traci/TraCIAPI.h"
#include <omnetpp/simtime.h>

namespace traci
{

struct TraCITime
{
    constexpr static SUMOTime min() { return 0; }
    constexpr static SUMOTime max() { return 0x7fffffff; }

    SUMOTime milliseconds;
};

inline omnetpp::SimTime time_cast(SUMOTime sumo)
{
    return omnetpp::SimTime { sumo, omnetpp::SIMTIME_MS };
}

} // namespace traci

#endif /* TIME_H_TRBXBVQM */

