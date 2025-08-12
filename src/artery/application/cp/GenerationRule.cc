/*
 * Artery V2X Simulation Framework
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/application/cp/GenerationRule.h"

#include "artery/envmod/EnvironmentModelObject.h"
#include "artery/utility/Geometry.h"

#include <boost/units/cmath.hpp>
#include <boost/units/systems/si/io.hpp>
#include <omnetpp/clog.h>
#include <vanetza/facilities/cam_functions.hpp>
#include <vanetza/units/time.hpp>


namespace opp = omnetpp;


namespace artery
{
namespace cp
{

Define_Module(StaticGeneration);

void StaticGeneration::initialize()
{
    cSimpleModule::initialize();

    EV_TRACE << "sizeof this: " << sizeof(*this) << "\n";
}

std::pair<PocCandidates, bool> StaticGeneration::getPocCandidates(LocalTracking& localTracking, const omnetpp::SimTime& now, const omnetpp::SimTime& next)
{
    auto pocs = std::make_pair(PocCandidates(), true);
    pocs.first.reserve(localTracking.size());
    for (auto& localObject : localTracking) { pocs.first.emplace_back(&localObject); }

    return pocs;
}


Define_Module(DynamicGeneration);

DynamicGeneration::DynamicGeneration() :
    mDistanceDeltaThreshold(vanetza::units::Length(4.0 * vanetza::units::si::meter)),
    mSpeedDeltaThreshold(vanetza::units::Velocity(0.5 * vanetza::units::si::meter_per_second)),
    mHeadingDeltaThreshold(vanetza::units::Angle(4.0 * vanetza::units::degree)),
    mTimeDeltaThreshold(1000, opp::SIMTIME_MS),
    mDynamicPrediction(false),
    mPredictionInterval(opp::SimTime::ZERO)
{
}


void DynamicGeneration::initialize()
{
    cSimpleModule::initialize();

    mDistanceDeltaThreshold = vanetza::units::Length(par("distanceDeltaThreshold").doubleValue() * vanetza::units::si::meter);
    mSpeedDeltaThreshold = vanetza::units::Velocity(par("speedDeltaThreshold").doubleValue() * vanetza::units::si::meter_per_second);
    mHeadingDeltaThreshold = vanetza::units::Angle(par("headingDeltaThreshold").doubleValue() * vanetza::units::degree);
    // The SimTime constructor that takes a cPar does not evaluate the unit and always uses seconds, construct manually to apply the correct unit
    mTimeDeltaThreshold = opp::SimTime(par("timeDeltaThreshold").intValue(), opp::SIMTIME_MS);
    const auto dpIdx = findPar("dynamicPrediction");
    if (dpIdx != -1) {
        mDynamicPrediction = par(dpIdx);
    }
    const auto piIdx = findPar("predictionInterval");
    if (piIdx != -1) {
        // The SimTime constructor that takes a cPar does not evaluate the unit and always uses seconds, construct manually to apply the correct unit
        mPredictionInterval = opp::SimTime(par(piIdx).intValue(), opp::SIMTIME_MS);
    }

    EV_TRACE << "Distance: " << mDistanceDeltaThreshold << "\n";
    EV_TRACE << "Speed: " << mSpeedDeltaThreshold << "\n";
    EV_TRACE << "Heading: " << mHeadingDeltaThreshold << "\n";
    EV_TRACE << "Time: " << mTimeDeltaThreshold.ustr() << "\n";
    EV_TRACE << "Prediction: " << mPredictionInterval.ustr() << ", dynamic: " << mDynamicPrediction << "\n";

    EV_TRACE << "sizeof this: " << sizeof(*this) << "\n";
}


std::pair<PocCandidates, bool> DynamicGeneration::getPocCandidates(LocalTracking& localTracking, const omnetpp::SimTime& now, const omnetpp::SimTime& next)
{
    // TODO: The conditions for object class person or animal are not honored
    // Objects for the next interval must only be included if there are any for the current interval,
    // to prevent a two pass over the tracking data store the pocs separate and unite them at the end
    // if there are any for the current interval.
    const auto predictionInterval = (mPredictionInterval.isZero() ? opp::SimTime::ZERO : (mDynamicPrediction ? next - now : mPredictionInterval));
    auto pocsNow = std::make_pair(PocCandidates(), false);
    PocCandidates pocsNext;
    pocsNow.first.reserve(localTracking.size());
    if (!predictionInterval.isZero()) {
        pocsNext.reserve(localTracking.size());
    }

    for (auto& localObject : localTracking) {
        // If no dynamic data is present this is a new object and must be included
        if (localObject.second.hasPrevDynamics()) {
            const auto vehicle = localObject.first.lock();
            const auto& data = vehicle->getVehicleData();

            if (!exceedsAny(now, data, localObject.second.getPrevDynamics())) {
                if (!predictionInterval.isZero()) {
                    const auto timestamp = now + predictionInterval;
                    const auto position = predictPosition(data.position(), data.heading(), data.speed(), predictionInterval);
                    const auto speed = predictSpeed(data.speed(), data.acceleration(), predictionInterval);

                    if (exceedsAny(TrackedDynamics{timestamp, position, speed, data.heading()}, localObject.second.getPrevDynamics())) {
                        pocsNext.emplace_back(&localObject);
                    }
                }
                continue;
            }  // else qualified for this CPM, include
        }

        pocsNow.first.emplace_back(&localObject);
    }

    if (!pocsNow.first.empty() && !pocsNext.empty()) {
        pocsNow.first.insert(pocsNow.first.end(), pocsNext.begin(), pocsNext.end());
    }

    return pocsNow;
}


bool DynamicGeneration::exceedsAny(const omnetpp::SimTime& now, const artery::VehicleDataProvider& current, const TrackedDynamics& previous) const
{
    return (
        exceedsDistance(current.position(), previous.position) || exceedsSpeed(current.speed(), previous.speed) ||
        exceedsHeading(current.heading(), previous.heading) || exceedsTime(now, previous.timestamp));
}

bool DynamicGeneration::exceedsAny(const TrackedDynamics& current, const TrackedDynamics& previous) const
{
    return (
        exceedsDistance(current.position, previous.position) || exceedsSpeed(current.speed, previous.speed) ||
        exceedsHeading(current.heading, previous.heading) || exceedsTime(current.timestamp, previous.timestamp));
}


bool DynamicGeneration::exceedsDistance(const artery::Position& current, const artery::Position& previous) const
{
    return (artery::distance(current, previous) > mDistanceDeltaThreshold);
}

bool DynamicGeneration::exceedsSpeed(const vanetza::units::Velocity& current, const vanetza::units::Velocity& previous) const
{
    return (abs(current - previous) > mSpeedDeltaThreshold);
}

bool DynamicGeneration::exceedsHeading(const vanetza::units::Angle& current, const vanetza::units::Angle& previous) const
{
    return !vanetza::facilities::similar_heading(current, previous, mHeadingDeltaThreshold);
}

bool DynamicGeneration::exceedsTime(const omnetpp::SimTime& current, const omnetpp::SimTime& previous) const
{
    return (current - previous > mTimeDeltaThreshold);
}


artery::Position DynamicGeneration::predictPosition(
    const artery::Position& position, const vanetza::units::Angle& heading, const vanetza::units::Velocity& speed, const omnetpp::SimTime& interval) const
{
    // The angle between the heading and the x-axis is 90 degrees(0.5 Pi) - heading
    // because the heading is given in relation to north but the x-axis grows to east.
    // The y-value has to be negated because sin is defined at the unit circle
    // with the y-axis pointing upwards but the y-axis grows downwards.
    const auto direction = artery::Position(
        cos(vanetza::units::Angle(0.5 * PI * vanetza::units::si::radians) - heading) * speed *
            vanetza::units::Duration(interval.dbl() * vanetza::units::si::second),
        -(sin(vanetza::units::Angle(0.5 * PI * vanetza::units::si::radians) - heading) * speed *
          vanetza::units::Duration(interval.dbl() * vanetza::units::si::second)));

    return artery::Position(position.x + direction.x, position.y + direction.y);
}

vanetza::units::Velocity DynamicGeneration::predictSpeed(
    const vanetza::units::Velocity& speed, const vanetza::units::Acceleration& acceleration, const omnetpp::SimTime& interval) const
{
    return vanetza::units::Velocity(speed + acceleration * vanetza::units::Duration(interval.dbl() * vanetza::units::si::second));
}

}  // namespace cp
}  // namespace artery
