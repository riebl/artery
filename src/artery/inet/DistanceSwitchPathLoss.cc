/*
* Artery V2X Simulation Framework
* Copyright 2018 Raphael Riebl
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#include "artery/inet/DistanceSwitchPathLoss.h"
#include <omnetpp/checkandcast.h>

namespace artery
{

Define_Module(DistanceSwitchPathLoss)

void DistanceSwitchPathLoss::initialize()
{
    mThreshold = inet::m { par("thresholdDistance") };
    if (mThreshold <= inet::m { 0.0 }) {
        error("threshold distance has to be positive (given: %d)", mThreshold.get());
    }

    mModelNear = omnetpp::check_and_cast<inet::physicallayer::IPathLoss*>(getSubmodule("near"));
    mModelFar = omnetpp::check_and_cast<inet::physicallayer::IPathLoss*>(getSubmodule("far"));
}

double DistanceSwitchPathLoss::computePathLoss(const inet::physicallayer::ITransmission* transmission, const inet::physicallayer::IArrival* arrival) const
{
    const inet::m distance { transmission->getStartPosition().distance(arrival->getStartPosition()) };
    if (distance < mThreshold) {
        return mModelNear->computePathLoss(transmission, arrival);
    } else {
        return mModelFar->computePathLoss(transmission, arrival);
    }
}

double DistanceSwitchPathLoss::computePathLoss(inet::mps propagation, inet::Hz frequency, inet::m distance) const
{
    if (distance < mThreshold) {
        return mModelNear->computePathLoss(propagation, frequency, distance);
    } else {
        return mModelFar->computePathLoss(propagation, frequency, distance);
    }
}

inet::m DistanceSwitchPathLoss::computeRange(inet::mps propagation, inet::Hz frequency, double loss) const
{
    const inet::m rangeNear = mModelNear->computeRange(propagation, frequency, loss);
    const inet::m rangeFar = mModelFar->computeRange(propagation, frequency, loss);
    return rangeNear < rangeFar ? rangeFar : rangeNear;
}

} // namespace artery
