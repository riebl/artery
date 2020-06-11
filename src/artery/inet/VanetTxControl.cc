/*
* Artery V2X Simulation Framework
* Copyright 2020 Raphael Riebl
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#include "artery/inet/VanetTxControl.h"

namespace artery
{

VanetTxControl::VanetTxControl(const VanetTxControl& other) :
    inet::Ieee802Ctrl(other),
    mTxRequest(other.getTransmissionRequest() ? other.getTransmissionRequest()->dup() : nullptr)
{
}

VanetTxControl& VanetTxControl::operator=(const VanetTxControl& other)
{
    if (&other != this) {
        inet::Ieee802Ctrl::operator=(other);
        const TransmissionRequest* request = other.getTransmissionRequest();
        mTxRequest.reset(request ? request->dup() : nullptr);
    }
    return *this;
}

} // namespace artery
