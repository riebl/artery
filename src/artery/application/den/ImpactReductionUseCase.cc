/*
 * Artery V2X Simulation Framework
 * Copyright 2016-2018 Raphael Riebl, Christina Obermaier
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/application/DenmObject.h"
#include "artery/application/DenService.h"
#include "artery/application/den/ImpactReductionUseCase.h"
#include "artery/application/StoryboardSignal.h"
#include "artery/application/VehicleDataProvider.h"
#include <omnetpp/cexception.h>

Define_Module(artery::den::ImpactReductionContainerExchange)

namespace artery
{
namespace den
{

void ImpactReductionContainerExchange::check()
{
    if (mPendingRequest) {
        transmitMessage(RequestResponseIndication_request);
        mPendingRequest = false;
    }
}

void ImpactReductionContainerExchange::indicate(const artery::DenmObject& denm)
{
    if (denm & CauseCode::CollisionRisk) {
        const vanetza::asn1::Denm& asn1 = denm.asn1();
        if (asn1->denm.alacarte && asn1->denm.alacarte->impactReduction) {
            auto& indication = asn1->denm.alacarte->impactReduction->requestResponseIndication;
            if (indication == RequestResponseIndication_request) {
                transmitMessage(RequestResponseIndication_response);
            }
        }
    }
}

void ImpactReductionContainerExchange::transmitMessage(RequestResponseIndication_t ind)
{
    auto denm = createMessage(ind);
    auto request = createRequest();
    mService->sendDenm(std::move(denm), request);
}

void ImpactReductionContainerExchange::handleStoryboardTrigger(const StoryboardSignal& signal)
{
    if (signal.getCause() == "irc") {
        mPendingRequest = true;
    }
}

vanetza::asn1::Denm ImpactReductionContainerExchange::createMessage(RequestResponseIndication_t ind)
{
    auto msg = createMessageSkeleton();
    msg->denm.management.relevanceDistance = vanetza::asn1::allocate<RelevanceDistance_t>();
    *msg->denm.management.relevanceDistance = RelevanceDistance_lessThan100m;
    msg->denm.management.relevanceTrafficDirection = vanetza::asn1::allocate<RelevanceTrafficDirection_t>();
    *msg->denm.management.relevanceTrafficDirection = RelevanceTrafficDirection_allTrafficDirections;
    msg->denm.management.validityDuration = vanetza::asn1::allocate<ValidityDuration_t>();
    *msg->denm.management.validityDuration = 2;
    msg->denm.management.stationType = StationType_unknown; // TODO retrieve type from SUMO

    msg->denm.situation = vanetza::asn1::allocate<SituationContainer_t>();
    msg->denm.situation->informationQuality = 1;
    msg->denm.situation->eventType.causeCode = CauseCodeType_collisionRisk;
    msg->denm.situation->eventType.subCauseCode = 0;

    msg->denm.alacarte = vanetza::asn1::allocate<AlacarteContainer_t>();
    msg->denm.alacarte->impactReduction = vanetza::asn1::allocate<ImpactReductionContainer_t>();
    ImpactReductionContainer_t& irc = *msg->denm.alacarte->impactReduction;
    // TODO approximate values from SUMO vehicle type
    irc.heightLonCarrLeft = HeightLonCarr_unavailable;
    irc.heightLonCarrRight = HeightLonCarr_unavailable;
    irc.posLonCarrLeft = PosLonCarr_unavailable;
    irc.posLonCarrRight = PosLonCarr_unavailable;
    PosPillar_t* pillar = vanetza::asn1::allocate<PosPillar_t>();
    *pillar = PosPillar_unavailable;
    ASN_SEQUENCE_ADD(&irc.positionOfPillars, pillar);
    irc.posCentMass = PosCentMass_unavailable;
    irc.wheelBaseVehicle = WheelBaseVehicle_unavailable;
    irc.turningRadius = TurningRadius_unavailable;
    irc.positionOfOccupants.buf = static_cast<uint8_t*>(vanetza::asn1::allocate(3));
    irc.positionOfOccupants.size = 3;
    irc.positionOfOccupants.bits_unused = 4;
    irc.positionOfOccupants.buf[0] |= 1 << (7 - PositionOfOccupants_row1LeftOccupied);
    irc.positionOfOccupants.buf[1] |= 1 << (15 - PositionOfOccupants_row2NotDetectable);
    irc.positionOfOccupants.buf[1] |= 1 << (15 - PositionOfOccupants_row3NotPresent);
    irc.positionOfOccupants.buf[2] |= 1 << (23 - PositionOfOccupants_row4NotPresent);
    irc.posFrontAx = PosFrontAx_unavailable;
    irc.vehicleMass = VehicleMass_unavailable;
    irc.requestResponseIndication = ind;

    return msg;
}

vanetza::btp::DataRequestB ImpactReductionContainerExchange::createRequest()
{
    namespace geonet = vanetza::geonet;
    using vanetza::units::si::seconds;
    using vanetza::units::si::meter;

    vanetza::btp::DataRequestB request;
    request.gn.traffic_class.tc_id(0);
    request.gn.maximum_hop_limit = 1;

    geonet::DataRequest::Repetition repetition;
    repetition.interval = 0.1 * seconds;
    repetition.maximum = 0.3 * seconds;
    request.gn.repetition = repetition;

    geonet::Area destination;
    geonet::Circle destination_shape;
    destination_shape.r = 100.0 * meter;
    destination.shape = destination_shape;
    destination.position.latitude = mVdp->latitude();
    destination.position.longitude = mVdp->longitude();
    request.gn.destination = destination;

    return request;
}

} // namespace den
} // namespace artery
