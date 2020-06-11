/*
* Artery V2X Simulation Framework
* Copyright 2019-2020 Raphael Riebl
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#ifndef ARTERY_VANETRECEIVER_H_PNEYWKVR
#define ARTERY_VANETRECEIVER_H_PNEYWKVR

#include "inet/physicallayer/ieee80211/packetlevel/Ieee80211ScalarReceiver.h"

namespace artery
{

/**
 * VanetReceiver adds capturing of a stronger frame while already receiving a weak frame
 */
class VanetReceiver : public inet::physicallayer::Ieee80211ScalarReceiver
{
public:
    bool computeIsReceptionAttempted(const inet::physicallayer::IListening*, const inet::physicallayer::IReception*,
            inet::physicallayer::IRadioSignal::SignalPart, const inet::physicallayer::IInterference*) const override;

protected:
    void initialize(int stage) override;
    const inet::physicallayer::ReceptionIndication* computeReceptionIndication(const inet::physicallayer::ISNIR*) const override;

private:
    double mCaptureThreshold;
};

} // namespace artery

#endif /* ARTERY_VANETRECEIVER_H_PNEYWKVR */

