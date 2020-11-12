/*
* Artery V2X Simulation Framework
* Copyright 2020 Raphael Riebl
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#include "artery/application/den/TractionLoss.h"
#include "artery/application/DenService.h"
#include "artery/application/StoryboardSignal.h"
#include "artery/application/VehicleDataProvider.h"

namespace artery
{
namespace den
{

Define_Module(artery::den::TractionLoss)

void TractionLoss::initialize(int stage)
{
    UseCase::initialize(stage);
}

void TractionLoss::handleStoryboardTrigger(const StoryboardSignal& signal)
{
    if (signal.getCause() == "traction loss") {
        mPendingSignal = true;
    }
}

void TractionLoss::check()
{
    if (!isDetectionBlocked() && mPendingSignal) {
        blockDetection();
        mPendingSignal = false;
        auto message = createMessage();
        auto request = createRequest();
        mService->sendDenm(std::move(message), request);
    }
}

vanetza::asn1::Denm TractionLoss::createMessage()
{
    auto msg = createMessageSkeleton();
    msg->denm.management.relevanceDistance = vanetza::asn1::allocate<RelevanceDistance_t>();
    *msg->denm.management.relevanceDistance = RelevanceDistance_lessThan100m;
    msg->denm.management.relevanceTrafficDirection = vanetza::asn1::allocate<RelevanceTrafficDirection_t>();
    *msg->denm.management.relevanceTrafficDirection = RelevanceTrafficDirection_allTrafficDirections;
    msg->denm.management.validityDuration = vanetza::asn1::allocate<ValidityDuration_t>();
    *msg->denm.management.validityDuration = 600;
    msg->denm.management.stationType = StationType_unknown; // TODO retrieve type from SUMO

    msg->denm.situation = vanetza::asn1::allocate<SituationContainer_t>();
    msg->denm.situation->informationQuality = 0;
    msg->denm.situation->eventType.causeCode = CauseCodeType_adverseWeatherCondition_Adhesion;
    msg->denm.situation->eventType.subCauseCode = 0;
    // TODO: add position traces
    return msg;
}

vanetza::btp::DataRequestB TractionLoss::createRequest()
{
    namespace gn = vanetza::geonet;
    using vanetza::units::si::seconds;
    using vanetza::units::si::meter;

    vanetza::btp::DataRequestB request;
    request.gn.traffic_class.tc_id(1);

    gn::DataRequest::Repetition repetition;
    repetition.interval = 1.0 * seconds;
    repetition.maximum = 300.0 * seconds;
    request.gn.repetition = repetition;

    gn::Area destination;
    gn::Circle shape;
    shape.r = 1000.0 * meter;
    destination.shape = shape;
    destination.position.latitude = mVdp->latitude();
    destination.position.longitude = mVdp->longitude();
    request.gn.destination = destination;
    request.gn.maximum_lifetime = gn::Lifetime { gn::Lifetime::Base::Ten_Seconds, 60 };

    return request;
}

} // namespace den
} // namespace artery
