#ifndef ARTERY_RUNTIME_H_BHZUYBWF
#define ARTERY_RUNTIME_H_BHZUYBWF

#include "artery/application/Timer.h"
#include <omnetpp/csimplemodule.h>
#include <omnetpp/simtime.h>
#include <vanetza/common/manual_runtime.hpp>

namespace artery
{

class Runtime : public omnetpp::cSimpleModule, public vanetza::Runtime
{
public:
    Runtime() = default;
    Runtime(const Runtime&) = delete;
    Runtime& operator=(const Runtime&) = delete;
    ~Runtime();

    // cSimpleModule
    void initialize(int stage) override;
    void handleMessage(omnetpp::cMessage*) override;

    // Runtime
    void schedule(vanetza::Clock::time_point tp, const Callback& cb, const void* scope = nullptr) override;
    void schedule(vanetza::Clock::duration d, const Callback& cb, const void* scope = nullptr) override;
    void cancel(const void* scope) override;
    vanetza::Clock::time_point now() const override;

private:
    omnetpp::SimTime convertSimTime(vanetza::Clock::time_point tp) const;
    void schedule();
    void update();

    Timer mTimer;
    vanetza::ManualRuntime mRuntime;
    omnetpp::cMessage* mUpdateEvent = nullptr;
    omnetpp::SimTime mLastUpdate;
};

} // namespace artery

#endif /* ARTERY_RUNTIME_H_BHZUYBWF */

