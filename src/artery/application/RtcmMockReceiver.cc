/*
* Artery V2X Simulation Framework
* Copyright 2020 Raphael Riebl et al.
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#include "artery/application/RtcmMockReceiver.h"
#include "artery/application/RtcmMockMessage.h"
#include <omnetpp/checkandcast.h>

using namespace omnetpp;

namespace artery
{

Define_Module(RtcmMockReceiver)

namespace {
const simsignal_t rtcmReceivedSignal = cComponent::registerSignal("RtcmReceived");
} // namespace

void RtcmMockReceiver::indicate(const vanetza::btp::DataIndication&, cPacket* packet)
{
    auto rtcm = check_and_cast<RtcmMockMessage*>(packet);
    emit(rtcmReceivedSignal, rtcm);
    delete packet;
}

} // namespace artery
