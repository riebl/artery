/*
 * Artery V2X Simulation Framework
 * Copyright 2016-2017 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_IMPACTREDUCTIONUSECASE_H_FE8Z3BLJ
#define ARTERY_IMPACTREDUCTIONUSECASE_H_FE8Z3BLJ

#include "artery/application/DenmUseCase.h"

class StoryboardSignal;
class VehicleDataProvider;

namespace artery
{

class DenmObject;

class ImpactReductionContainerExchange : public denm::UseCase
{
public:
    ImpactReductionContainerExchange(const VehicleDataProvider&);

    bool handleMessageReception(const DenmObject&) override;
    void handleStoryboardTrigger(const StoryboardSignal&) override;

    void message(vanetza::asn1::Denm&) override;
    void dissemination(vanetza::btp::DataRequestB&) override;

protected:
    void update() override;
    bool checkPreconditions() override;
    bool checkConditions() override;

private:
    enum class Trigger {
        NONE, REQUEST, RESPONSE
    };

    const VehicleDataProvider& mVehicleDataProvider;
    Trigger mTrigger;
};

} // namespace artery

#endif /* ARTERY_IMPACTREDUCTIONUSECASE_H_FE8Z3BLJ */

