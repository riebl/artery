#ifndef API_H_HBQVASFR
#define API_H_HBQVASFR

#include "traci/sumo/utils/traci/TraCIAPI.h"
#include "traci/Angle.h"
#include "traci/Boundary.h"
#include "traci/Position.h"
#include "traci/Time.h"
#include <omnetpp/simtime.h>

namespace traci
{

class ServerEndpoint;

class API : public TraCIAPI
{
public:
    using Version = std::pair<int, std::string>;
    Version getVersion() const;

    void connect(const ServerEndpoint&);
};

} // namespace traci

#endif /* API_H_HBQVASFR */

