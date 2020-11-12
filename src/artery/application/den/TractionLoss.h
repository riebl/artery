/*
* Artery V2X Simulation Framework
* Copyright 2020 Raphael Riebl
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#ifndef ARTERY_DEN_TRACTIONLOSS_H_EVLJFJUZ
#define ARTERY_DEN_TRACTIONLOSS_H_EVLJFJUZ

#include "artery/application/den/SuspendableUseCase.h"
#include <vanetza/btp/data_request.hpp>

namespace artery
{
namespace den
{

class TractionLoss : public SuspendableUseCase
{
public:
    vanetza::btp::DataRequestB createRequest();
    vanetza::asn1::Denm createMessage();

    void check() override;
    void indicate(const artery::DenmObject&) override {};
    void handleStoryboardTrigger(const StoryboardSignal&) override;

protected:
    void initialize(int stage) override;

private:
    bool mPendingSignal = false;
};

} // namespace den
} // namespace artery

#endif /* ARTERY_DEN_TRACTIONLOSS_H_EVLJFJUZ */

