#ifndef TIME_H_TRBXBVQM
#define TIME_H_TRBXBVQM

#include "traci/sumo/utils/traci/TraCIAPI.h"
#include <omnetpp/simtime.h>

namespace traci
{

struct Time
{
    // SUMO 1.0 still uses integer for internal time representation
    // Hence, DBL_MAX is not an appropriate upper bound
    constexpr static int min() { return 0; }
    constexpr static int max() { return 0x7fffffff; }

    explicit Time(double s) : seconds(s) {}
    operator omnetpp::SimTime() { return omnetpp::SimTime(seconds); }

    double seconds;
};

} // namespace traci

#endif /* TIME_H_TRBXBVQM */

