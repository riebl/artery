/*
* Artery V2X Simulation Framework
* Copyright 2020 Raphael Riebl
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#include "artery/storyboard/DeferringCondition.h"
#include "artery/utility/PointerCheck.h"

namespace artery
{

DeferringCondition::DeferringCondition(omnetpp::cRNG* rng, omnetpp::SimTime min, omnetpp::SimTime max) :
    mRNG(notNullPtr(rng)), mMinDeferral(min), mMaxDeferral(max)
{
}

ConditionResult DeferringCondition::testCondition(const Vehicle& v)
{
    const omnetpp::SimTime now = omnetpp::simTime();
    bool result = false;

    auto found = mDeferrals.find(&v);
    if (found != mDeferrals.end()) {
        result = found->second <= now;
    } else {
        omnetpp::SimTime deferral = now + omnetpp::uniform(mRNG, mMinDeferral, mMaxDeferral);
        result = (deferral <= now);
        mDeferrals[&v] = deferral;
    }

    return result;
}

} // namespace artery
