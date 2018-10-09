#include "artery/networking/FsmDccEntity.h"
#include "artery/utility/InitStages.h"
#include <vanetza/dcc/bursty_transmit_rate_control.hpp>
#include <vanetza/dcc/fully_meshed_state_machine.hpp>
#include <vanetza/dcc/gradual_state_machine.hpp>
#include <vanetza/dcc/single_reactive_transmit_rate_control.hpp>

namespace artery
{

Define_Module(FsmDccEntity)

void FsmDccEntity::finish()
{
    // free those objects before runtime vanishes
    mTransmitRateControl.reset();
    mStateMachine.reset();
    DccEntityBase::finish();
}

void FsmDccEntity::onGlobalCbr(vanetza::dcc::ChannelLoad cbr)
{
    mStateMachine->update(cbr);
}

void FsmDccEntity::initializeTransmitRateControl()
{
    initializeStateMachine(par("StateMachine"));
    initializeTransmitRateControl(par("TransmitRateControl"));
}

void FsmDccEntity::initializeStateMachine(const std::string& name)
{
    using namespace vanetza::dcc;

    if (name == "FullyMeshed") {
        mStateMachine.reset(new FullyMeshedStateMachine());
    } else if (name == "Gradual 500us") {
        mStateMachine.reset(new GradualStateMachine(etsiStates500us));
    } else if (name == "Gradual 1ms") {
        mStateMachine.reset(new GradualStateMachine(etsiStates1ms));
    } else {
        error("Unknown DCC state machine \"%s\"", name.c_str());
    }
}

void FsmDccEntity::initializeTransmitRateControl(const std::string& name)
{
    ASSERT(mStateMachine);
    ASSERT(mRuntime);
    using namespace vanetza::dcc;

    if (name == "Bursty") {
        mTransmitRateControl.reset(new BurstyTransmitRateControl(*mStateMachine, *mRuntime));
    } else if (name == "StateMachine") {
        mTransmitRateControl.reset(new SingleReactiveTransmitRateControl(*mStateMachine, *mRuntime));
    } else {
        error("Unknown DCC Transmit Rate Control \"%s\"", name.c_str());
    }
}

} // namespace artery
