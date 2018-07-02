/*
 * Artery V2X Simulation Framework
 * Copyright 2016-2018 Raphael Riebl, Christina Obermaier
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_DEN_IMPACTREDUCTIONCONTAINEREXCHANGE_H_FE8Z3BLJ
#define ARTERY_DEN_IMPACTREDUCTIONCONTAINEREXCHANGE_H_FE8Z3BLJ

#include "artery/application/den/UseCase.h"

namespace artery
{
namespace den
{

class ImpactReductionContainerExchange : public UseCase
{
public:
    // UseCase interface
    void check() override;
    void indicate(const artery::DenmObject&) override;
    void handleStoryboardTrigger(const StoryboardSignal&) override;

    vanetza::asn1::Denm createMessage(RequestResponseIndication_t);
    vanetza::btp::DataRequestB createRequest();

private:
    bool mPendingRequest = false;
    void transmitMessage(RequestResponseIndication_t);
};

} // namespace den
} // namespace artery

#endif /* ARTERY_DEN_IMPACTREDUCTIONCONTAINEREXCHANGE_H_FE8Z3BLJ */

