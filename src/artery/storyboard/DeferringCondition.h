/*
* Artery V2X Simulation Framework
* Copyright 2020 Raphael Riebl
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#ifndef ARTERY_DEFERRINGCONDITION_H_DR3LWRVJ
#define ARTERY_DEFERRINGCONDITION_H_DR3LWRVJ

#include "artery/storyboard/Condition.h"
#include <omnetpp/crng.h>
#include <omnetpp/simtime.h>
#include <unordered_map>

namespace artery
{

/**
 * Defer triggering of a condition by a random delay.
 *
 * The delay is drawn uniformly from the given [min; max) range once per vehicle.
 * The condition evaluates to true after the delay expired relative to the first evaluation's time point.
 */
class STORYBOARD_API DeferringCondition : public Condition
{
public:
    DeferringCondition(omnetpp::cRNG*, omnetpp::SimTime min, omnetpp::SimTime max);
    ConditionResult testCondition(const Vehicle&) override;

private:
    omnetpp::cRNG* mRNG;
    omnetpp::SimTime mMinDeferral;
    omnetpp::SimTime mMaxDeferral;
    std::unordered_map<const Vehicle*, omnetpp::SimTime> mDeferrals;
};

} // namespace artery

#endif /* ARTERY_DEFERRINGCONDITION_H_DR3LWRVJ */

