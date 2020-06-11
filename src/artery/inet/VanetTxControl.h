/*
* Artery V2X Simulation Framework
* Copyright 2020 Raphael Riebl
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#ifndef ARTERY_VANETTXCONTROL_H_NXB5FIPT
#define ARTERY_VANETTXCONTROL_H_NXB5FIPT

#include <inet/linklayer/common/Ieee802Ctrl.h>
#include <inet/physicallayer/ieee80211/packetlevel/Ieee80211ControlInfo_m.h>
#include <memory>

namespace artery
{

class VanetTxControl : public inet::Ieee802Ctrl
{
public:
    using TransmissionRequest = inet::physicallayer::Ieee80211TransmissionRequest;

    VanetTxControl() = default;
    VanetTxControl(const VanetTxControl&);
    VanetTxControl& operator=(const VanetTxControl&);
    VanetTxControl* dup() const override { return new VanetTxControl(*this); }

    void setTransmissionRequest(TransmissionRequest* tx) { mTxRequest.reset(tx); }
    TransmissionRequest* getTransmissionRequest() { return mTxRequest.get(); }
    const TransmissionRequest* getTransmissionRequest() const { return mTxRequest.get(); }
    TransmissionRequest* removeTransmissionRequest() { return mTxRequest.release(); }

private:
    std::unique_ptr<TransmissionRequest> mTxRequest;
};

} // namespace artery

#endif /* ARTERY_VANETTXCONTROL_H_NXB5FIPT */

