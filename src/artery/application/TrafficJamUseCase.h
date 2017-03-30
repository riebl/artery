/*
 * Artery V2X Simulation Framework
 * Copyright 2016-2017 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_TRAFFICJAMUSECASE_H_TN78N6QA
#define ARTERY_TRAFFICJAMUSECASE_H_TN78N6QA

#include "artery/application/DenmMemory.h"
#include "artery/application/DenmUseCase.h"
#include "artery/application/Sampling.h"
#include "artery/application/VehicleDataProvider.h"
#include <vanetza/units/velocity.hpp>

namespace artery
{

// forward declaration
class LocalDynamicMap;


/**
 * Check triggering conditions for "Dangerous End Of Queue" use case.
 * See release 1.1.0 of C2C-CC Triggering Conditions "Traffic Jam" (Version 3.3.0)
 */
class TrafficJamEndOfQueue : public denm::UseCase
{
public:
    TrafficJamEndOfQueue(const VehicleDataProvider&, const denm::Memory&);

    /**
     * Switch if on-board map or camera sensor shall report a "non-urban environment".
     * A non-urban environment is the precondition for this use case, i.e. DENMs are only generated when set to true.
     * \param flag true if vehicle is assumed to be in an non-urban environment
     */
    void setNonUrbanEnvironment(bool flag) { mNonUrbanEnvironment = flag; }

    void message(vanetza::asn1::Denm&) override;
    void dissemination(vanetza::btp::DataRequestB&) override;

protected:
    void update() override;
    bool checkPreconditions() override;
    bool checkConditions() override;
    bool checkEgoDeceleration() const;
    bool checkEndOfQueueReceived() const;
    bool checkJamAheadReceived() const;

private:
    const VehicleDataProvider& mVehicleDataProvider;
    const denm::Memory& mDenmMemory;
    bool mNonUrbanEnvironment;
    SkipEarlySampler<vanetza::units::Velocity> mVelocitySampler;
};

class TrafficJamAhead : public denm::UseCase
{
public:
    TrafficJamAhead(const VehicleDataProvider&, const denm::Memory&, const LocalDynamicMap&);

    /**
     * Switch if on-board map or camera sensor shall report a "non-urban environment".
     * A non-urban environment is the precondition for this use case, i.e. DENMs are only generated when set to true.
     * \param flag true if vehicle is assumed to be in an non-urban environment
     */
    void setNonUrbanEnvironment(bool flag) { mNonUrbanEnvironment = flag; }

    void message(vanetza::asn1::Denm&) override;
    void dissemination(vanetza::btp::DataRequestB&) override;

protected:
    void update() override;
    bool checkPreconditions() override;
    bool checkConditions() override;
    bool checkLowAverageEgoVelocity() const;
    bool checkStationaryEgo() const;
    bool checkTrafficJamAheadReceived() const;
    bool checkSlowVehiclesAheadByV2X() const;

private:
    const VehicleDataProvider& mVehicleDataProvider;
    const denm::Memory& mDenmMemory;
    const LocalDynamicMap& mLocalDynamicMap;
    bool mNonUrbanEnvironment;
    unsigned mUpdateCounter;
    SkipEarlySampler<vanetza::units::Velocity> mVelocitySampler;
};

} // namespace artery

#endif /* ARTERY_TRAFFICJAMUSECASE_H_TN78N6QA */

