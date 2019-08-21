/*
* Artery V2X Simulation Framework
* Copyright 2019 Raphael Riebl
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#include "artery/inet/VanetReceiver.h"
#include "inet/physicallayer/contract/packetlevel/IRadio.h"
#include "inet/physicallayer/contract/packetlevel/IRadioMedium.h"

namespace artery
{

Define_Module(VanetReceiver)

namespace phy = inet::physicallayer;

void VanetReceiver::initialize(int stage)
{
    phy::Ieee80211ScalarReceiver::initialize(stage);
    if (stage == inet::INITSTAGE_LOCAL) {
        mCaptureThreshold = inet::math::dB2fraction(par("captureThreshold"));
    }
}

bool VanetReceiver::computeIsReceptionAttempted(const phy::IListening* listening, const phy::IReception* reception,
            phy::IRadioSignal::SignalPart part, const phy::IInterference* interference) const
{
    if (!computeIsReceptionPossible(listening, reception, part)) {
        return false; // cannot even hear this
    } else if (omnetpp::simTime() == reception->getStartTime(part)) {
        auto transmission = reception->getReceiver()->getReceptionInProgress();
        if (transmission == nullptr || transmission == reception->getTransmission()) {
            return true; // already receiving this signal or no reception is yet in progress
        } else {
            // allow capturing of stronger signals
            auto medium = reception->getReceiver()->getMedium();
            auto snir = medium->getSNIR(reception->getReceiver(), reception->getTransmission())->getMin();
            return snir > mCaptureThreshold;
        }
    } else {
        return true; // let's try, why not?
    }
}

} // namespace artery
