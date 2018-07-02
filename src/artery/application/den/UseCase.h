/*
 * Artery V2X Simulation Framework
 * Copyright 2018 Christina Obermaier
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef DEN_USE_CASE_H_
#define DEN_USE_CASE_H_

#include <boost/optional.hpp>
#include <omnetpp/csimplemodule.h>
#include <omnetpp/simtime.h>
#include <vanetza/asn1/denm.hpp>
#include <functional>

class StoryboardSignal;
class VehicleDataProvider;

namespace artery
{

class DenmObject;
class DenService;

namespace den
{
class UseCase : public omnetpp::cSimpleModule
{
public:
    /**
     * Evaluate use case triggering conditions
     * Generates DENM if neccessary
     * \note invoked for each middleware time step (trigger)
     */
    virtual void check() = 0;

    /**
     * Forwards the message received by DenService to the UseCase
     * Messages responding to this can be triggered immediately
     */
    virtual void indicate(const artery::DenmObject&) = 0;

    /**
     * Forwards the received Storyboard trigger signal to the UseCase
     * \note Do not invoke message sending in this method!
     * \note Delay sending until check() is called again.
     * \note This avoids station synchronization.
     */
    virtual void handleStoryboardTrigger(const StoryboardSignal&) = 0;

protected:
    using TriggeringCondition = std::function<bool(void)>;

    void initialize(int) override;
    int numInitStages () const override { return 1; }

    virtual vanetza::asn1::Denm createMessageSkeleton();

    void setDetectionBlockingTime(omnetpp::SimTime);
    void setDetectionBlockingSince(omnetpp::SimTime);
    bool isDetectionBlocked();

    DenService* mService = nullptr;
    const VehicleDataProvider* mVdp = nullptr;

private:
    omnetpp::SimTime mDetectionBlockingTime;
    boost::optional<omnetpp::SimTime> mDetectionBlockingSince;
};

} // namespace den
} // namespace artery

#endif /* DEN_USE_CASE_H_ */
