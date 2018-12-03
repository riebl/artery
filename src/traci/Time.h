#ifndef TIME_H_TRBXBVQM
#define TIME_H_TRBXBVQM

#include "traci/sumo/utils/traci/TraCIAPI.h"
#include <omnetpp/simtime.h>

namespace traci
{

struct Time
{
    explicit Time(double s) : seconds(s) {}
    operator omnetpp::SimTime() { return omnetpp::SimTime(seconds); }

    double seconds;
};

} // namespace traci

#endif /* TIME_H_TRBXBVQM */

