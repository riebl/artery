/*
* Artery V2X Simulation Framework
* Copyright 2020 Raphael Riebl
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#include "artery/storyboard/LikelihoodCondition.h"
#include "artery/utility/PointerCheck.h"
#include <algorithm>

namespace artery
{

LikelihoodCondition::LikelihoodCondition(omnetpp::cRNG* rng, double likelihood) :
    mLikelihood(std::max(0.0, std::min(1.0, likelihood))),
    mRng(notNullPtr(rng))
{
}

ConditionResult LikelihoodCondition::testCondition(const Vehicle& v)
{
    bool condition = false;

    auto found = mDecisions.find(&v);
    if (found != mDecisions.end()) {
        condition = found->second;
    } else {
        condition = omnetpp::uniform(mRng, 0.0, 1.0) < mLikelihood;
        mDecisions[&v] = condition;
    }

    return condition;
}

} // namespace artery
