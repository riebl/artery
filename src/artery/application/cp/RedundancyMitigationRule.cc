/*
 * Artery V2X Simulation Framework
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/application/cp/RedundancyMitigationRule.h"

#include "artery/application/VehicleDataProvider.h"
#include "artery/envmod/EnvironmentModelObject.h"
#include "artery/utility/Geometry.h"

#include <boost/units/cmath.hpp>
#include <boost/units/systems/si/io.hpp>
#include <omnetpp/clog.h>

#include <algorithm>


namespace opp = omnetpp;


namespace artery
{
namespace cp
{

Define_Module(DistanceRedundancy);

DistanceRedundancy::DistanceRedundancy() :
    mWindowRedundancy(1000, opp::SIMTIME_MS), mDistanceRedundancy(vanetza::units::Length(10.0 * vanetza::units::si::meter))
{
}


void DistanceRedundancy::initialize()
{
    cSimpleModule::initialize();

    // The SimTime constructor that takes a cPar does not evaluate the unit and always uses seconds, construct manually to apply the correct unit
    mWindowRedundancy = opp::SimTime(par("windowRedundancy").intValue(), opp::SIMTIME_MS);
    mDistanceRedundancy = vanetza::units::Length(par("distanceRedundancy").doubleValue() * vanetza::units::si::meter);

    EV_TRACE << "Window: " << mWindowRedundancy.ustr() << "\n";
    EV_TRACE << "Distance: " << mDistanceRedundancy << "\n";

    EV_TRACE << "sizeof this: " << sizeof(*this) << "\n";
}


PocCandidates& DistanceRedundancy::filterPocCandidates(
    PocCandidates& pocCandidates, const RemoteTracking& remoteTracking, const artery::VehicleDataProvider& self, const omnetpp::SimTime& now)
{
    const auto threshold = now - mWindowRedundancy;
    const auto distance = mDistanceRedundancy;
    pocCandidates.erase(
        std::remove_if(
            pocCandidates.begin(), pocCandidates.end(),
            [&remoteTracking, &self, &threshold, distance](const PocCandidates::value_type& poc) {
                auto objectTracking = remoteTracking.find(poc->first.lock()->getVehicleData().getStationId());
                if (objectTracking != remoteTracking.end()) {
                    for (const auto& stationTracking : objectTracking->second.getCpmDynamics()) {
                        for (const auto& cpmDynamics : stationTracking.second) {
                            if (cpmDynamics.received < threshold) {
                                continue;
                            }
                            if (artery::distance(self.position(), cpmDynamics.senderPosition) <= distance) {
                                return true;
                            }
                        }
                    }
                }
                return false;
            }),
        pocCandidates.end());

    return pocCandidates;
}


Define_Module(DynamicsRedundancy);

DynamicsRedundancy::DynamicsRedundancy() :
    mDistanceRedundancy(vanetza::units::Length(4.0 * vanetza::units::si::meter)),
    mSpeedRedundancy(vanetza::units::Velocity(0.5 * vanetza::units::si::meter_per_second))
{
}


void DynamicsRedundancy::initialize()
{
    cSimpleModule::initialize();

    mDistanceRedundancy = vanetza::units::Length(par("distanceRedundancy").doubleValue() * vanetza::units::si::meter);
    mSpeedRedundancy = vanetza::units::Velocity(par("speedRedundancy").doubleValue() * vanetza::units::si::meter_per_second);

    EV_TRACE << "Distance: " << mDistanceRedundancy << "\n";
    EV_TRACE << "Speed: " << mSpeedRedundancy << "\n";

    EV_TRACE << "sizeof this: " << sizeof(*this) << "\n";
}


PocCandidates& DynamicsRedundancy::filterPocCandidates(
    PocCandidates& pocCandidates, const RemoteTracking& remoteTracking, const artery::VehicleDataProvider& self, const omnetpp::SimTime& now)
{
    const auto distance = mDistanceRedundancy;
    const auto speed = mSpeedRedundancy;
    pocCandidates.erase(
        std::remove_if(
            pocCandidates.begin(), pocCandidates.end(),
            [&remoteTracking, distance, speed](const PocCandidates::value_type& poc) {
                const auto object = poc->first.lock();
                const auto& vehicle = object->getVehicleData();

                auto objectTracking = remoteTracking.find(vehicle.getStationId());
                if (objectTracking != remoteTracking.end()) {
                    if (objectTracking->second.hasCpmDynamics()) {
                        const auto& latestDynamics = objectTracking->second.getLatestAggregateCpmDynamics();
                        return (
                            artery::distance(vehicle.position(), latestDynamics.position) <= distance && abs(vehicle.speed() - latestDynamics.speed) <= speed);
                    }
                }
                return false;
            }),
        pocCandidates.end());

    return pocCandidates;
}


Define_Module(FrequencyRedundancy);

FrequencyRedundancy::FrequencyRedundancy() : mWindowRedundancy(1000, opp::SIMTIME_MS), mNumberRedundancy(5)
{
}


void FrequencyRedundancy::initialize()
{
    cSimpleModule::initialize();

    // The SimTime constructor that takes a cPar does not evaluate the unit and always uses seconds, construct manually to apply the correct unit
    mWindowRedundancy = opp::SimTime(par("windowRedundancy").intValue(), opp::SIMTIME_MS);
    mNumberRedundancy = par("numberRedundancy");

    EV_TRACE << "Window: " << mWindowRedundancy.ustr() << "\n";
    EV_TRACE << "Number: " << mNumberRedundancy << "\n";

    EV_TRACE << "sizeof this: " << sizeof(*this) << "\n";
}


PocCandidates& FrequencyRedundancy::filterPocCandidates(
    PocCandidates& pocCandidates, const RemoteTracking& remoteTracking, const artery::VehicleDataProvider& self, const omnetpp::SimTime& now)
{
    const auto threshold = now - mWindowRedundancy;
    const auto number = mNumberRedundancy;
    pocCandidates.erase(
        std::remove_if(
            pocCandidates.begin(), pocCandidates.end(),
            [&remoteTracking, &threshold, number](const PocCandidates::value_type& poc) {
                auto objectTracking = remoteTracking.find(poc->first.lock()->getVehicleData().getStationId());
                if (objectTracking != remoteTracking.end()) {
                    int objectCount = 0;
                    for (const auto& stationTracking : objectTracking->second.getCpmDynamics()) {
                        for (const auto& cpmDynamics : stationTracking.second) {
                            if (cpmDynamics.received < threshold) {
                                continue;
                            }
                            ++objectCount;
                            if (objectCount > number) {
                                return true;
                            }
                        }
                    }
                }
                return false;
            }),
        pocCandidates.end());

    return pocCandidates;
}


Define_Module(SelfAnnouncementRedundancy);

void SelfAnnouncementRedundancy::initialize()
{
    cSimpleModule::initialize();

    EV_TRACE << "sizeof this: " << sizeof(*this) << "\n";
}


PocCandidates& SelfAnnouncementRedundancy::filterPocCandidates(
    PocCandidates& pocCandidates, const RemoteTracking& remoteTracking, const artery::VehicleDataProvider& self, const omnetpp::SimTime& now)
{
    pocCandidates.erase(
        std::remove_if(
            pocCandidates.begin(), pocCandidates.end(),
            [&remoteTracking](const PocCandidates::value_type& poc) {
                return (remoteTracking.find(poc->first.lock()->getVehicleData().getStationId()) != remoteTracking.end());
            }),
        pocCandidates.end());

    return pocCandidates;
}

}  // namespace cp
}  // namespace artery
