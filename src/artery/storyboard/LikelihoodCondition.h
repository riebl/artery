/*
* Artery V2X Simulation Framework
* Copyright 2020 Raphael Riebl
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#ifndef ARTERY_LIKELIHOODCONDITION_H_2D1RVCBH
#define ARTERY_LIKELIHOODCONDITION_H_2D1RVCBH

#include "artery/storyboard/Condition.h"
#include <omnetpp/crng.h>
#include <unordered_map>

namespace artery
{

/**
 * LikelihoodCondition becomes only true by chance for a vehicle.
 *
 * If this condition evaluates to true for a particular vehicle is only determined once.
 * The likelihood has to be given in the range [0.0; 1.0].
 */
class LikelihoodCondition : public Condition
{
public:
    LikelihoodCondition(omnetpp::cRNG*, double likelihood);
    ConditionResult testCondition(const Vehicle&) override;

private:
    const double mLikelihood;
    omnetpp::cRNG* mRng;
    std::unordered_map<const Vehicle*, bool> mDecisions;
};

} // namespace artery

#endif /* ARTERY_LIKELIHOODCONDITION_H_2D1RVCBH */

