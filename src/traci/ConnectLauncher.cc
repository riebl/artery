#include "traci/ConnectLauncher.h"

namespace traci
{

Define_Module(ConnectLauncher)

void ConnectLauncher::initialize()
{
    m_endpoint.hostname = par("hostname").stringValue();
    m_endpoint.port = par("port");
    m_endpoint.clientId = par("clientId");
}

ServerEndpoint ConnectLauncher::launch()
{
    return m_endpoint;
}

} // namespace traci
