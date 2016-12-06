/*
 * Artery V2X Simulation Framework
 * Copyright 2016-2017 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/application/DenmUseCase.h"
#include <omnetpp/csimulation.h>

namespace artery
{
namespace denm
{

UseCase::UseCase() :
    mDetectionBlockingTime(omnetpp::SimTime::ZERO)
{
}

bool UseCase::check()
{
    update();
    bool trigger = false;
    if (!isDetectionBlocked() && checkPreconditions() && checkConditions()) {
        trigger = true;
        mDetectionBlockingSince = omnetpp::simTime();
    }
}

void UseCase::setDetectionBlockingTime(omnetpp::SimTime block)
{
    mDetectionBlockingTime = std::max(omnetpp::SimTime::ZERO, block);
}

bool UseCase::isDetectionBlocked()
{
    const auto now = omnetpp::simTime();
    bool blocked = false;

    if (mDetectionBlockingSince) {
        if (*mDetectionBlockingSince + mDetectionBlockingTime < now) {
            mDetectionBlockingSince.reset();
        } else {
            blocked = true;
        }
    }

    return blocked;
}

void UseCase::update()
{
}

} // namespace denm
} // namespace artery
