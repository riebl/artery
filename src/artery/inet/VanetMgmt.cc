/*
* Artery V2X Simulation Framework
* Copyright 2020 Raphael Riebl
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#include "artery/inet/VanetMgmt.h"
#include "artery/inet/VanetRxControl.h"
#include "artery/inet/VanetTxControl.h"

namespace artery
{

Define_Module(VanetMgmt)

inet::ieee80211::Ieee80211DataFrame* VanetMgmt::encapsulate(omnetpp::cPacket* msg)
{
    inet::physicallayer::Ieee80211TransmissionRequest* tx = nullptr;
    VanetTxControl* ctrl = dynamic_cast<VanetTxControl*>(msg->getControlInfo());
    if (ctrl) {
        tx = ctrl->removeTransmissionRequest();
    }

    auto frame = Ieee80211MgmtAdhoc::encapsulate(msg);
    if (tx) {
        frame->setControlInfo(tx);
    }
    return frame;
}

omnetpp::cPacket* VanetMgmt::decapsulate(inet::ieee80211::Ieee80211DataFrame* frame)
{
    VanetRxControl* ctrl = new VanetRxControl();
    ctrl->setSrc(frame->getTransmitterAddress());
    ctrl->setDest(frame->getReceiverAddress());
    int tid = frame->getTid();
    if (tid < 8) {
        ctrl->setUserPriority(tid);
    }
    if (auto snap = dynamic_cast<inet::ieee80211::Ieee80211DataFrameWithSNAP*>(frame)) {
        ctrl->setEtherType(snap->getEtherType());
    }
    if (auto indication = dynamic_cast<inet::physicallayer::Ieee80211ReceptionIndication*>(frame->getControlInfo())) {
        frame->removeControlInfo();
        ctrl->setReceptionIndication(indication);
    }

    omnetpp::cPacket* payload = frame->decapsulate();
    payload->setControlInfo(ctrl);
    delete frame;
    return payload;
}

} // namespace artery
