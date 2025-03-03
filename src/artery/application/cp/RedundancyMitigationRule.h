/*
 * Artery V2X Simulation Framework
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_CP_REDUNDANCYMITIGATIONRULE_H_
#define ARTERY_CP_REDUNDANCYMITIGATIONRULE_H_

/**
 * @file RedundancyMitigationRule.h
 * @brief Implementation of Redundancy Mitigation Rules
 */

#include "artery/application/cp/Tracking.h"

#include <omnetpp/csimplemodule.h>
#include <omnetpp/simtime.h>
#include <vanetza/units/length.hpp>
#include <vanetza/units/velocity.hpp>


namespace artery
{
class VehicleDataProvider;
}  // namespace artery


namespace artery
{
namespace cp
{

/**
 * @brief Redundancy Mitigation Rule interface
 */
class RedundancyMitigationRule
{
public:
    virtual ~RedundancyMitigationRule() = default;

    /**
     * @brief Filter the given PocCandidates objects for CPM inclusion
     *
     * @param pocCandidates All currently for CPM inclusion selected objects, MUST NOT contain any expired objects.
     *                      The objects themself will not be modified but not matching objects will be removed.
     * @param remoteTracking All objects known by remote tracking, MUST NOT contain any expired objects
     * @param self Dynamic properties of the Ego Vehicle
     * @param now Generation timestamp of the current CPM
     * @return The given pocCandidates reference
     */
    virtual PocCandidates& filterPocCandidates(
        PocCandidates& pocCandidates, const RemoteTracking& remoteTracking, const artery::VehicleDataProvider& self, const omnetpp::SimTime& now) = 0;
};


/**
 * @brief Distance-based Redundancy Mitigation Rule
 *
 * Implemented according to ETSI TR 103 562 V2.1.1.
 */
class DistanceRedundancy : public omnetpp::cSimpleModule, public RedundancyMitigationRule
{
public:
    DistanceRedundancy();

    void initialize() override;

    PocCandidates& filterPocCandidates(
        PocCandidates& pocCandidates, const RemoteTracking& remoteTracking, const artery::VehicleDataProvider& self, const omnetpp::SimTime& now) override;

private:
    // *** NED defined parameters ***
    omnetpp::SimTime mWindowRedundancy;
    vanetza::units::Length mDistanceRedundancy;
    // ***
};


/**
 * @brief Dynamics-based Redundancy Mitigation Rule
 *
 * Implemented according to ETSI TR 103 562 V2.1.1.
 */
class DynamicsRedundancy : public omnetpp::cSimpleModule, public RedundancyMitigationRule
{
public:
    DynamicsRedundancy();

    void initialize() override;

    PocCandidates& filterPocCandidates(
        PocCandidates& pocCandidates, const RemoteTracking& remoteTracking, const artery::VehicleDataProvider& self, const omnetpp::SimTime& now) override;

private:
    // *** NED defined parameters ***
    vanetza::units::Length mDistanceRedundancy;
    vanetza::units::Velocity mSpeedRedundancy;
    // ***
};


/**
 * @brief Frequency-based Redundancy Mitigation Rule
 *
 * Implemented according to ETSI TR 103 562 V2.1.1.
 */
class FrequencyRedundancy : public omnetpp::cSimpleModule, public RedundancyMitigationRule
{
public:
    FrequencyRedundancy();

    void initialize() override;

    PocCandidates& filterPocCandidates(
        PocCandidates& pocCandidates, const RemoteTracking& remoteTracking, const artery::VehicleDataProvider& self, const omnetpp::SimTime& now) override;

private:
    // *** NED defined parameters ***
    omnetpp::SimTime mWindowRedundancy;
    int mNumberRedundancy;
    // ***
};


/**
 * @brief Object Self-Announcement Redundancy Mitigation Rule
 *
 * Implemented according to ETSI TR 103 562 V2.1.1.
 */
class SelfAnnouncementRedundancy : public omnetpp::cSimpleModule, public RedundancyMitigationRule
{
public:
    void initialize() override;

    PocCandidates& filterPocCandidates(
        PocCandidates& pocCandidates, const RemoteTracking& remoteTracking, const artery::VehicleDataProvider& self, const omnetpp::SimTime& now) override;
};

}  // namespace cp
}  // namespace artery

#endif /* ARTERY_CP_REDUNDANCYMITIGATIONRULE_H_ */
