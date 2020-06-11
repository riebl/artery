/*
* Artery V2X Simulation Framework
* Copyright 2020 Raphael Riebl
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#ifndef ARTERY_VANETHCF_H_FSAYCGUD
#define ARTERY_VANETHCF_H_FSAYCGUD

#include <inet/linklayer/ieee80211/mac/coordinationfunction/Hcf.h>

namespace artery
{

class VanetHcf : public inet::ieee80211::Hcf
{
protected:
    void setFrameMode(inet::ieee80211::Ieee80211Frame*, const inet::physicallayer::IIeee80211Mode*) const override;
};

} // namespace artery

#endif /* ARTERY_VANETHCF_H_FSAYCGUD */

