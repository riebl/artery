#ifndef LAUNCHER_H_NAC0X8JG
#define LAUNCHER_H_NAC0X8JG

#include <string>

namespace traci
{

struct ServerEndpoint
{
    std::string hostname;
    int port;
    int clientId = 1;
    bool retry = false;
};

class Launcher
{
public:

    virtual ~Launcher() = default;
    virtual ServerEndpoint launch() = 0;
};

} // namespace traci

#endif /* LAUNCHER_H_NAC0X8JG */

