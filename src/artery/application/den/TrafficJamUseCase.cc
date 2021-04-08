/*
 * Artery V2X Simulation Framework
 * Copyright 2016-2018 Raphael Riebl, Christina Obermaier
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/application/DenService.h"
#include "artery/application/LocalDynamicMap.h"
#include "artery/application/den/TrafficJamUseCase.h"
#include "artery/application/SampleBufferAlgorithm.h"
#include <boost/units/base_units/metric/hour.hpp>
#include <boost/units/systems/si/length.hpp>
#include <boost/units/systems/si/time.hpp>
#include <boost/units/systems/si/prefixes.hpp>
#include <omnetpp/csimulation.h>
#include <vanetza/btp/data_request.hpp>
#include <vanetza/facilities/cam_functions.hpp>
#include <vanetza/units/acceleration.hpp>
#include <vanetza/units/time.hpp>
#include <vanetza/units/velocity.hpp>
#include <algorithm>
#include <numeric>

static const auto hour = 3600.0 * boost::units::si::seconds;
static const auto km_per_hour = boost::units::si::kilo * boost::units::si::meter / hour;

using omnetpp::SIMTIME_S;
using omnetpp::SIMTIME_MS;

Define_Module(artery::den::TrafficJamEndOfQueue)
Define_Module(artery::den::TrafficJamAhead)

namespace artery
{
namespace den
{

void TrafficJamEndOfQueue::initialize(int stage)
{
    UseCase::initialize(stage);
    if (stage == 0)
    {
        mNonUrbanEnvironment = par("nonUrbanEnvironment").boolValue();
        mDenmMemory = mService->getMemory();
        mVelocitySampler.setDuration(par("sampleDuration"));
        mVelocitySampler.setInterval(par("sampleInterval"));
    }
}

void TrafficJamEndOfQueue::check()
{
    mVelocitySampler.feed(mVdp->speed(), mVdp->updated());
    if (!isDetectionBlocked() && checkPreconditions() && checkConditions())
    {
        blockDetection();
        auto message = createMessage();
        auto request = createRequest();
        mService->sendDenm(std::move(message), request);
    }
}

bool TrafficJamEndOfQueue::checkPreconditions()
{
    return mNonUrbanEnvironment;
}

bool TrafficJamEndOfQueue::checkConditions()
{
    const TriggeringCondition tc0 = std::bind(&TrafficJamEndOfQueue::checkEgoDeceleration, this);
    const bool tc1 = false; // there are no passengers in ego vehicle enabling hazard lights, assume false
    const bool tc2 = false; // so far no simulated vehicle enables hazard lights, assume false
    const TriggeringCondition tc3 = std::bind(&TrafficJamEndOfQueue::checkEndOfQueueReceived, this);
    const TriggeringCondition tc4 = std::bind(&TrafficJamEndOfQueue::checkJamAheadReceived, this);
    const bool tc5 = false; // so far there are no emergency vehicles in simulation, assume false
    const bool tc6 = false; // simulated vehicle has no on-board sensors for end of queue detection, assume false

    return (tc1 && tc2) || (tc0() && (tc2 || tc3() || tc4() || tc5 || tc6));
}

bool TrafficJamEndOfQueue::checkEgoDeceleration() const
{
    using vanetza::units::Acceleration;
    using vanetza::units::Duration;
    using vanetza::units::Velocity;
    using boost::units::si::seconds;
    using boost::units::si::meter_per_second_squared;

    static const Velocity targetVelocityThreshold { 30.0 * km_per_hour };
    static const Velocity initialVelocityThreshold { 80.0 * km_per_hour };
    static const Acceleration initialDecelThreshold { -0.1 * meter_per_second_squared };
    static const Duration instantDecelDuration { 10.0 * seconds };
    static const Acceleration instantDecelThreshold { -3.5 * meter_per_second_squared };

    bool fulfilled = false;
    const auto& velocitySamples = mVelocitySampler.buffer();

    // current velocity shall not exceed target velocity
    if (!velocitySamples.empty() && velocitySamples.latest().value <= targetVelocityThreshold) {
        // find the newest sample above initial velocity threshold
        auto initialVelocity = std::find_if(std::next(velocitySamples.begin()), velocitySamples.end(),
                [](const Sample<Velocity>& s) { return s.value >= initialVelocityThreshold; });
        if (initialVelocity != velocitySamples.end()) {
            // should never fail because only 10s are buffered at all
            assert(duration(*initialVelocity, velocitySamples.latest()) < instantDecelDuration);
            fulfilled = differentiate(*initialVelocity, velocitySamples.latest()) < instantDecelThreshold;
        }
    }

    return fulfilled;
}

bool TrafficJamEndOfQueue::checkEndOfQueueReceived() const
{
    // TODO relevance check for ego vehicle is missing
    return mDenmMemory->count(CauseCode::DangerousEndOfQueue) >= 1;
}

bool TrafficJamEndOfQueue::checkJamAheadReceived() const
{
    // TODO relevance check for ego vehicle is missing
    return mDenmMemory->count(CauseCode::TrafficCondition) >= 5;
}

vanetza::asn1::Denm TrafficJamEndOfQueue::createMessage()
{
    auto msg = createMessageSkeleton();
    msg->denm.management.relevanceDistance = vanetza::asn1::allocate<RelevanceDistance_t>();
    *msg->denm.management.relevanceDistance = RelevanceDistance_lessThan1000m;
    msg->denm.management.relevanceTrafficDirection = vanetza::asn1::allocate<RelevanceTrafficDirection_t>();
    *msg->denm.management.relevanceTrafficDirection = RelevanceTrafficDirection_upstreamTraffic;
    msg->denm.management.validityDuration = vanetza::asn1::allocate<ValidityDuration_t>();
    *msg->denm.management.validityDuration = 20;
    msg->denm.management.stationType = StationType_unknown; // TODO retrieve type from SUMO

    msg->denm.situation = vanetza::asn1::allocate<SituationContainer_t>();
    msg->denm.situation->informationQuality = 1;
    msg->denm.situation->eventType.causeCode = CauseCodeType_dangerousEndOfQueue;
    msg->denm.situation->eventType.subCauseCode = 0;

    // TODO set road type in Location container
    // TODO set lane position in Alacarte container
    return msg;
}

vanetza::btp::DataRequestB TrafficJamEndOfQueue::createRequest()
{
    namespace geonet = vanetza::geonet;
    using vanetza::units::si::seconds;
    using vanetza::units::si::meter;

    vanetza::btp::DataRequestB request;
    request.gn.traffic_class.tc_id(1);

    geonet::DataRequest::Repetition repetition;
    repetition.interval = 0.5 * seconds;
    repetition.maximum = 20.0 * seconds;
    request.gn.repetition = repetition;

    geonet::Area destination;
    geonet::Circle destination_shape;
    destination_shape.r = 1000.0 * meter;
    destination.shape = destination_shape;
    destination.position.latitude = mVdp->latitude();
    destination.position.longitude = mVdp->longitude();
    request.gn.destination = destination;

    return request;
}


void TrafficJamAhead::initialize(int stage)
{
    UseCase::initialize(stage);
    if (stage == 0) {
        mNonUrbanEnvironment = par("nonUrbanEnvironment").boolValue();
        mDenmMemory = mService->getMemory();
        mVelocitySampler.setDuration(par("sampleDuration"));
        mVelocitySampler.setInterval(par("sampleInterval"));
        mUpdateCounter = 0;
        mLocalDynamicMap = &mService->getFacilities().get_const<LocalDynamicMap>();
    }
}

void TrafficJamAhead::check()
{
    mVelocitySampler.feed(mVdp->speed(), mVdp->updated());
    if (!isDetectionBlocked() && checkPreconditions() && checkConditions())
    {
        blockDetection();
        auto message = createMessage();
        auto request = createRequest();
        mService->sendDenm(std::move(message), request);
    }
}

bool TrafficJamAhead::checkPreconditions()
{
    // TODO check for absence of stationary and special vehicle warnings
    return mNonUrbanEnvironment;
}

bool TrafficJamAhead::checkConditions()
{
    const TriggeringCondition tc0 = std::bind(&TrafficJamAhead::checkLowAverageEgoVelocity, this);
    const TriggeringCondition tc1 = std::bind(&TrafficJamAhead::checkStationaryEgo, this);
    const TriggeringCondition tc2 = std::bind(&TrafficJamAhead::checkTrafficJamAheadReceived, this);
    const bool tc3 = false; // no mobile radio equipment available (yet)
    const TriggeringCondition tc4 = std::bind(&TrafficJamAhead::checkSlowVehiclesAheadByV2X, this);
    const bool tc5 = false; // no on-board sensors available (yet)

    return tc0() || (tc1() && (tc2() || tc3 || tc4() || tc5));
}

bool TrafficJamAhead::checkLowAverageEgoVelocity() const
{
    static const omnetpp::SimTime avgWindowMax {120, SIMTIME_S};
    static const omnetpp::SimTime avgWindowMin {10, SIMTIME_S};
    const auto now = omnetpp::simTime();
    bool lowAvgEgoVelocity = false;

    using vanetza::units::Velocity;
    auto speedSamples = mVelocitySampler.buffer().not_before(now - avgWindowMax);
    if (speedSamples.duration() >= avgWindowMin) {
        static const Velocity zeroSpeed { 0.0 * km_per_hour };
        static const Velocity upperSpeed { 30.0 * km_per_hour };
        const Velocity speedAvg = average(speedSamples);
        lowAvgEgoVelocity = speedAvg <= upperSpeed && speedAvg > zeroSpeed;
    }
    return lowAvgEgoVelocity;
}

bool TrafficJamAhead::checkStationaryEgo() const
{
    using vanetza::units::Velocity;
    static const omnetpp::SimTime maxWindow {30, SIMTIME_S};
    static const omnetpp::SimTime minWindow {10, SIMTIME_S};
    const auto now = omnetpp::simTime();
    bool isStationary = false;
    auto speedSamples = mVelocitySampler.buffer().not_before(now - maxWindow);
    if (speedSamples.duration() >= minWindow) {
        static const Velocity zeroSpeed { 0.0 * km_per_hour };
        const Velocity speedAvg = average(speedSamples);
        isStationary = speedAvg <= zeroSpeed;
    }
    return isStationary;
}

bool TrafficJamAhead::checkTrafficJamAheadReceived() const
{
    // TODO relevance check is missing
    return mDenmMemory->count(CauseCode::TrafficCondition) >= 1;
}

bool TrafficJamAhead::checkSlowVehiclesAheadByV2X() const
{
    using vanetza::facilities::distance;
    using vanetza::facilities::similar_heading;

    LocalDynamicMap::CamPredicate slowVehicles = [&](const LocalDynamicMap::Cam& msg) {
        bool result = true;
        // less than 30 km/h, same driving direction and at most 100m distance
        const SpeedValue_t speedLimit = 833; // 833 cm/s are 29.988 km/h
        const vanetza::units::Angle headingLimit { 10.0 * vanetza::units::degree };
        const vanetza::units::Length distLimit { 100.0 * vanetza::units::si::meter };

        const auto& bc = msg->cam.camParameters.basicContainer;
        const auto& hfc = msg->cam.camParameters.highFrequencyContainer;
        if (hfc.present == HighFrequencyContainer_PR_basicVehicleContainerHighFrequency) {
            const auto& bvc = hfc.choice.basicVehicleContainerHighFrequency;
            const auto& vdp = *mVdp;
            if (bvc.speed.speedValue == SpeedValue_unavailable ||
                bvc.speed.speedValue > speedLimit * SpeedValue_oneCentimeterPerSec) {
                result = false;
            } else if (!similar_heading(bvc.heading, vdp.heading(), headingLimit)) {
                result = false;
            } else if (distance(bc.referencePosition, vdp.latitude(), vdp.longitude()) > distLimit) {
                return false;
            }
        } else {
            result = false;
        }

        return result;
    };
    return mLocalDynamicMap->count(slowVehicles) >= 5;
}

vanetza::asn1::Denm TrafficJamAhead::createMessage()
{
    auto msg = createMessageSkeleton();
    msg->denm.management.relevanceDistance = vanetza::asn1::allocate<RelevanceDistance_t>();
    *msg->denm.management.relevanceDistance = RelevanceDistance_lessThan1000m;
    msg->denm.management.relevanceTrafficDirection = vanetza::asn1::allocate<RelevanceTrafficDirection_t>();
    *msg->denm.management.relevanceTrafficDirection = RelevanceTrafficDirection_upstreamTraffic;
    msg->denm.management.validityDuration = vanetza::asn1::allocate<ValidityDuration_t>();
    *msg->denm.management.validityDuration = 60;
    msg->denm.management.stationType = StationType_unknown; // TODO retrieve type from SUMO

    msg->denm.situation = vanetza::asn1::allocate<SituationContainer_t>();
    msg->denm.situation->informationQuality = 1;
    msg->denm.situation->eventType.causeCode = CauseCodeType_trafficCondition;
    msg->denm.situation->eventType.subCauseCode = 0;

    // TODO set road type in Location container
    // TODO set lane position in Alacarte container

    return msg;
}

vanetza::btp::DataRequestB TrafficJamAhead::createRequest()
{
    namespace geonet = vanetza::geonet;
    using vanetza::units::si::seconds;
    using vanetza::units::si::meter;

    vanetza::btp::DataRequestB request;
    request.gn.traffic_class.tc_id(1);

    geonet::DataRequest::Repetition repetition;
    repetition.interval = 1.0 * seconds;
    repetition.maximum = 60.0 * seconds;

    request.gn.repetition = repetition;

    geonet::Area destination;
    geonet::Circle destination_shape;
    destination_shape.r = 1000.0 * meter;
    destination.shape = destination_shape;
    destination.position.latitude = mVdp->latitude();
    destination.position.longitude = mVdp->longitude();
    request.gn.destination = destination;

    return request;
}

} // namespace den
} // namespace artery
