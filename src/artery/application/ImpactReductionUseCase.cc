/*
 * Artery V2X Simulation Framework
 * Copyright 2016-2017 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/application/DenmObject.h"
#include "artery/application/ImpactReductionUseCase.h"
#include "artery/application/VehicleDataProvider.h"
#include "artery/storyboard/StoryboardSignal.h"

namespace artery
{

ImpactReductionContainerExchange::ImpactReductionContainerExchange(const VehicleDataProvider& vdp) :
    mVehicleDataProvider(vdp), mTrigger(Trigger::NONE)
{
}

bool ImpactReductionContainerExchange::handleMessageReception(const DenmObject& denm)
{
    if (denm & denm::CauseCode::CollisionRisk) {
        const vanetza::asn1::Denm& asn1 = denm.asn1();
        if (asn1->denm.alacarte && asn1->denm.alacarte->impactReduction) {
            auto& indication = asn1->denm.alacarte->impactReduction->requestResponseIndication;
            if (indication == RequestResponseIndication_request) {
                mTrigger = Trigger::RESPONSE;
            }
        }
    }

    return mTrigger == Trigger::RESPONSE;
}

void ImpactReductionContainerExchange::handleStoryboardTrigger(const StoryboardSignal& signal)
{
    if (signal.getCause() == "irc") {
        mTrigger = Trigger::REQUEST;
    }
}

void ImpactReductionContainerExchange::update()
{
}

bool ImpactReductionContainerExchange::checkPreconditions()
{
    // no preconditions for request (response requires only request reception)
    return true;
}

bool ImpactReductionContainerExchange::checkConditions()
{
    return mTrigger != Trigger::NONE;
}

void ImpactReductionContainerExchange::message(vanetza::asn1::Denm& msg)
{
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

    if (mTrigger == Trigger::REQUEST) {
        irc.requestResponseIndication = RequestResponseIndication_request;
    } else if (mTrigger == Trigger::RESPONSE) {
        irc.requestResponseIndication = RequestResponseIndication_response;
    } else {
        throw omnetpp::cRuntimeError("Impact Reduction Container creation requested without proper trigger");
    }
    mTrigger = Trigger::NONE;
}

void ImpactReductionContainerExchange::dissemination(vanetza::btp::DataRequestB& request)
{
    namespace geonet = vanetza::geonet;
    using vanetza::units::si::seconds;
    using vanetza::units::si::meter;

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
    destination.position.latitude = mVehicleDataProvider.latitude();
    destination.position.longitude = mVehicleDataProvider.longitude();
    request.gn.destination = destination;
}

} // namespace artery
