/*
* Artery V2X Simulation Framework
* Copyright 2020 Raphael Riebl
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#ifndef ARTERY_VANETRXCONTROL_H_YHJMCGWD
#define ARTERY_VANETRXCONTROL_H_YHJMCGWD

#include <inet/linklayer/common/Ieee802Ctrl.h>
#include <inet/physicallayer/ieee80211/packetlevel/Ieee80211ControlInfo_m.h>
#include <memory>

namespace artery
{

class VanetRxControl : public inet::Ieee802Ctrl
{
public:
    using ReceptionIndication = inet::physicallayer::Ieee80211ReceptionIndication;

    VanetRxControl() = default;
    VanetRxControl(const VanetRxControl&);
    VanetRxControl& operator=(const VanetRxControl&);
    VanetRxControl* dup() const override { return new VanetRxControl(*this); }

    void setReceptionIndication(ReceptionIndication* tx) { mRxIndication.reset(tx); }
    ReceptionIndication* getReceptionIndication() { return mRxIndication.get(); }
    const ReceptionIndication* getReceptionIndication() const { return mRxIndication.get(); }
    ReceptionIndication* removeReceptionIndication() { return mRxIndication.release(); }

private:
    std::unique_ptr<ReceptionIndication> mRxIndication;
};

} // namespace artery

#endif /* ARTERY_VANETRXCONTROL_H_YHJMCGWD */

