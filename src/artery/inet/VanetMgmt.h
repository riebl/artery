/*
* Artery V2X Simulation Framework
* Copyright 2020 Raphael Riebl
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#ifndef ARTERY_VANETMGMT_H_YLUKCHQT
#define ARTERY_VANETMGMT_H_YLUKCHQT

#include <inet/linklayer/ieee80211/mgmt/Ieee80211MgmtAdhoc.h>

namespace artery
{

class VanetMgmt : public inet::ieee80211::Ieee80211MgmtAdhoc
{
protected:
    inet::ieee80211::Ieee80211DataFrame* encapsulate(omnetpp::cPacket*) override;
    omnetpp::cPacket* decapsulate(inet::ieee80211::Ieee80211DataFrame*) override;
};

} // namespace artery

#endif /* ARTERY_VANETMGMT_H_YLUKCHQT */

