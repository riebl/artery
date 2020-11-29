#ifndef POSIXLAUNCHER_H_B3W5MDQW
#define POSIXLAUNCHER_H_B3W5MDQW

#include "traci/Launcher.h"
#include <omnetpp/csimplemodule.h>
#include <string>
#include <unistd.h>

namespace traci
{

class PosixLauncher : public Launcher, public omnetpp::cSimpleModule
{
public:
    PosixLauncher();
    ~PosixLauncher();
    ServerEndpoint launch() override;

protected:
    void initialize() override;
    void finish() override;

private:
    void kill();
    std::string command();
    int lookupPort();

    std::string m_executable;
    std::string m_command;
    std::string m_sumocfg;
    std::string m_extra_options;
    int m_port;
    int m_seed;
    pid_t m_pid;
};

} // namespace traci

#endif /* POSIXLAUNCHER_H_B3W5MDQW */

