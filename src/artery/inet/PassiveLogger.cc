#include "artery/inet/PassiveLogger.h"
#include <omnetpp/cmessage.h>

namespace artery
{

Define_Module(PassiveLogger)

void PassiveLogger::handleMessage(omnetpp::cMessage* msg)
{
    delete msg;
}

} // namespace artery
