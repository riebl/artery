#include "ConnectLauncher.h"

using namespace artery::traci;

Define_Module(ConnectLauncher)

void ConnectLauncher::initialize()
{
    endpoint_.hostname = par("hostname").stringValue();
    endpoint_.port = par("port");
    endpoint_.clientId = par("clientId");
}

ServerEndpoint ConnectLauncher::launch()
{
    return endpoint_;
}
