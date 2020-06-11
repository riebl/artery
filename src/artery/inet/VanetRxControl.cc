/*
* Artery V2X Simulation Framework
* Copyright 2020 Raphael Riebl
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#include "artery/inet/VanetRxControl.h"

namespace artery
{

VanetRxControl::VanetRxControl(const VanetRxControl& other) :
    inet::Ieee802Ctrl(other),
    mRxIndication(other.getReceptionIndication() ? other.getReceptionIndication()->dup() : nullptr)
{
}

VanetRxControl& VanetRxControl::operator=(const VanetRxControl& other)
{
    if (&other != this) {
        inet::Ieee802Ctrl::operator=(other);
        const ReceptionIndication* indication = other.getReceptionIndication();
        mRxIndication.reset(indication ? indication->dup() : nullptr);
    }
    return *this;
}

} // namespace artery
