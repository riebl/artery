/*
 * Artery V2X Simulation Framework
 * Copyright 2020 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/application/DenService.h"
#include "artery/application/den/EmergencyBrakeLight.h"
#include "artery/application/SampleBufferAlgorithm.h"
#include "artery/application/VehicleDataProvider.h"
#include <vanetza/asn1/its/DangerousSituationSubCauseCode.h>
#include <vanetza/btp/data_request.hpp>
#include <vanetza/units/acceleration.hpp>
#include <vanetza/units/velocity.hpp>
#include <algorithm>

namespace artery
{
namespace den
{

Define_Module(EmergencyBrakeLight)

void EmergencyBrakeLight::initialize(int stage)
{
    UseCase::initialize(stage);
    if (stage == 0)
    {
        using boost::units::si::meter_per_second;
        using boost::units::si::meter_per_second_squared;

        mAccelerationSampler.setDuration(par("sampleDuration"));
        mAccelerationSampler.setInterval(par("sampleInterval"));
        mSpeedThreshold = par("speedThreshold").doubleValue() * meter_per_second;
        mDecelerationThreshold = par("decelerationThreshold").doubleValue() * meter_per_second_squared;
    }
}

void EmergencyBrakeLight::check()
{
    mAccelerationSampler.feed(mVdp->acceleration(), mVdp->updated());
    if (!isDetectionBlocked() && checkConditions())
    {
        blockDetection();
        auto message = createMessage();
        auto request = createRequest();
        mService->sendDenm(std::move(message), request);
    }
}

bool EmergencyBrakeLight::checkConditions()
{
    return checkEgoSpeed() && checkEgoDeceleration();
}

bool EmergencyBrakeLight::checkEgoSpeed() const
{
    return mVdp->speed() > mSpeedThreshold;
}

bool EmergencyBrakeLight::checkEgoDeceleration() const
{
    const auto& samples = mAccelerationSampler.buffer();
    return samples.full() && std::all_of(samples.begin(), samples.end(),
        [this](const Sample<vanetza::units::Acceleration>& sample) -> bool {
            return sample.value < mDecelerationThreshold;
        });
}

vanetza::asn1::Denm EmergencyBrakeLight::createMessage()
{
    auto msg = createMessageSkeleton();
    msg->denm.management.relevanceDistance = vanetza::asn1::allocate<RelevanceDistance_t>();
    *msg->denm.management.relevanceDistance = RelevanceDistance_lessThan500m;
    msg->denm.management.relevanceTrafficDirection = vanetza::asn1::allocate<RelevanceTrafficDirection_t>();
    *msg->denm.management.relevanceTrafficDirection = RelevanceTrafficDirection_allTrafficDirections;
    msg->denm.management.validityDuration = vanetza::asn1::allocate<ValidityDuration_t>();
    *msg->denm.management.validityDuration = 2;
    msg->denm.management.stationType = StationType_unknown; // TODO retrieve type from SUMO

    msg->denm.situation = vanetza::asn1::allocate<SituationContainer_t>();
    msg->denm.situation->informationQuality = 1;
    msg->denm.situation->eventType.causeCode = CauseCodeType_dangerousSituation;
    msg->denm.situation->eventType.subCauseCode = DangerousSituationSubCauseCode_emergencyElectronicBrakeEngaged;

    // TODO set road type in Location container
    // TODO set lane position in Alacarte container
    return msg;
}

vanetza::btp::DataRequestB EmergencyBrakeLight::createRequest()
{
    namespace geonet = vanetza::geonet;
    using vanetza::units::si::seconds;
    using vanetza::units::si::meter;

    vanetza::btp::DataRequestB request;
    request.gn.traffic_class.tc_id(0);
    request.gn.maximum_lifetime = geonet::Lifetime { geonet::Lifetime::Base::One_Second, 2 };

    geonet::Area destination;
    geonet::Circle destination_shape;
    destination_shape.r = 500.0 * meter;
    destination.shape = destination_shape;
    destination.position.latitude = mVdp->latitude();
    destination.position.longitude = mVdp->longitude();
    request.gn.destination = destination;

    return request;
}

} // namespace den
} // namespace artery
