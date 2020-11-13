/*
* Artery V2X Simulation Framework
* Copyright 2020 Raphael Riebl et al.
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#include "artery/application/InfrastructureMockReceiver.h"
#include "artery/application/InfrastructureMockMessage.h"
#include <omnetpp/checkandcast.h>

using namespace omnetpp;

namespace artery
{

Define_Module(InfrastructureMockReceiver)

namespace {
    const simsignal_t immReceivedSignal = cComponent::registerSignal("ImmReceived");
} // namespace


void InfrastructureMockReceiver::indicate(const vanetza::btp::DataIndication&, cPacket* packet)
{
    auto msg = check_and_cast<InfrastructureMockMessage*>(packet);
    emit(immReceivedSignal, msg);
    delete packet;
}

} // namespace artery
