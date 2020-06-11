/*
* Artery V2X Simulation Framework
* Copyright 2020 Raphael Riebl
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#include "artery/inet/VanetHcf.h"

namespace artery
{

Define_Module(VanetHcf)

using namespace inet::ieee80211;
using inet::physicallayer::IIeee80211Mode;

void VanetHcf::setFrameMode(Ieee80211Frame* frame, const IIeee80211Mode* mode) const
{
    using namespace inet;

    auto ctrl = dynamic_cast<Ieee80211TransmissionRequest*>(frame->getControlInfo());
    if (ctrl) {
        // only use supplied mode (by rate selection module) if no mode has been requested explicitly
        if (!ctrl->getMode()) {
            ASSERT(mode != nullptr);
            ctrl->setMode(mode);
        }
    } else {
        Hcf::setFrameMode(frame, mode);
    }
}

} // namespace artery
