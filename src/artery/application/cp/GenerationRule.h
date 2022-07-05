/*
 * Artery V2X Simulation Framework
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_CP_GENERATIONRULE_H_
#define ARTERY_CP_GENERATIONRULE_H_

/**
 * @file GenerationRule.h
 * @brief Implementation of Generation Rules
 */

#include "artery/application/cp/Tracking.h"

#include <omnetpp/csimplemodule.h>
#include <omnetpp/simtime.h>
#include <vanetza/units/acceleration.hpp>
#include <vanetza/units/angle.hpp>
#include <vanetza/units/length.hpp>
#include <vanetza/units/velocity.hpp>

#include <utility>


namespace artery
{
struct Position;
class VehicleDataProvider;
}  // namespace artery


namespace artery
{
namespace cp
{

/**
 * @brief Generation Rule Interface
 */
class GenerationRule
{
public:
    virtual ~GenerationRule() = default;

    /**
     * @brief Filter the given LocalTracking objects for CPM inclusion
     *
     * @param localTracking All objects known by local tracking, MUST NOT contain any expired objects. These objects will not be modified.
     * @param now Generation timestamp of the current CPM
     * @param next Generation timestamp of the next CPM
     * @return Objects selected for CPM inclusion at timestamp now. These are pointers into localTracking, localTracking MUST be stable
     *         during the lifetime of these pointers. If the second parameter is true even zero Objects should create a CPM.
     */
    virtual std::pair<PocCandidates, bool> getPocCandidates(LocalTracking& localTracking, const omnetpp::SimTime& now, const omnetpp::SimTime& next) = 0;
};


/**
 * @brief Static Generation Rule
 *
 * Always returns all given LocalTracking objects.
 */
class StaticGeneration : public omnetpp::cSimpleModule, public GenerationRule
{
public:
    void initialize() override;

    std::pair<PocCandidates, bool> getPocCandidates(LocalTracking& localTracking, const omnetpp::SimTime& now, const omnetpp::SimTime& next) override;
};


/**
 * @brief Dynamic Generation Rule
 *
 * Returns LocalTracking objects according to the Perceived Object Container Inclusion Management
 * defined in ETSI TR 103 562 V2.1.1, optionally with prediction for the next CPM.
 *
 * TODO: Does not implement the case for objects of class person or animal
 */
class DynamicGeneration : public omnetpp::cSimpleModule, public GenerationRule
{
public:
    DynamicGeneration();

    void initialize() override;

    std::pair<PocCandidates, bool> getPocCandidates(LocalTracking& localTracking, const omnetpp::SimTime& now, const omnetpp::SimTime& next) override;

private:
    /**
     * @brief Test if any constraint is exceeded
     *
     * @param now Current timestamp
     * @param current Current dynamic properties
     * @param previous Previous dynamic properties
     * @return True, if any constraint is exceeded, false otherwise
     */
    bool exceedsAny(const omnetpp::SimTime& now, const artery::VehicleDataProvider& current, const TrackedDynamics& previous) const;
    /**
     * @brief Test if any constraint is exceeded
     *
     * @param current Current dynamic properties
     * @param previous Previous dynamic properties
     * @return True, if any constraint is exceeded, false otherwise
     */
    bool exceedsAny(const TrackedDynamics& current, const TrackedDynamics& previous) const;

    /**
     * @brief Test if the distance constraint is exceeded
     *
     * @param current Current position
     * @param previous Previous position
     * @return True, if the distance constraint is exceeded, false otherwise
     */
    bool exceedsDistance(const artery::Position& current, const artery::Position& previous) const;
    /**
     * @brief Test if the speed constraint is exceeded
     *
     * @param current Current speed
     * @param previous Previous speed
     * @return True, if the speed constraint is exceeded, false otherwise
     */
    bool exceedsSpeed(const vanetza::units::Velocity& current, const vanetza::units::Velocity& previous) const;
    /**
     * @brief Test if the heading constraint is exceeded
     *
     * @param current Current heading
     * @param previous Previous heading
     * @return True, if the heading constraint is exceeded, false otherwise
     */
    bool exceedsHeading(const vanetza::units::Angle& current, const vanetza::units::Angle& previous) const;
    /**
     * @brief Test if the time constraint is exceeded
     *
     * @param current Current time
     * @param previous Previous time
     * @return True, if the time constraint is exceeded, false otherwise
     */
    bool exceedsTime(const omnetpp::SimTime& current, const omnetpp::SimTime& previous) const;

    /**
     * @brief Predict future position assuming constant velocity
     *
     * @param position Current position
     * @param heading Current heading
     * @param speed Current speed
     * @param interval Prediction interval
     * @return Position after interval time has passed
     */
    artery::Position predictPosition(
        const artery::Position& position, const vanetza::units::Angle& heading, const vanetza::units::Velocity& speed, const omnetpp::SimTime& interval) const;
    /**
     * @brief Predict future speed assuming constant acceleration
     *
     * @param speed Current speed
     * @param acceleration Current acceleration
     * @param interval Prediction interval
     * @return Speed after interval time has passed
     */
    vanetza::units::Velocity predictSpeed(
        const vanetza::units::Velocity& speed, const vanetza::units::Acceleration& acceleration, const omnetpp::SimTime& interval) const;

private:
    // *** NED defined parameters ***
    vanetza::units::Length mDistanceDeltaThreshold;
    vanetza::units::Velocity mSpeedDeltaThreshold;
    vanetza::units::Angle mHeadingDeltaThreshold;
    omnetpp::SimTime mTimeDeltaThreshold;

    bool mDynamicPrediction;
    omnetpp::SimTime mPredictionInterval;
    // ***
};

}  // namespace cp
}  // namespace artery

#endif /* ARTERY_CP_GENERATIONRULE_H_ */
