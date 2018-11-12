#ifndef ARTERY_FSMDCCENTITY_H_FY8QUCBE
#define ARTERY_FSMDCCENTITY_H_FY8QUCBE

#include "artery/networking/DccEntityBase.h"
#include <vanetza/dcc/state_machine.hpp>
#include <vanetza/dcc/transmit_rate_control.hpp>
#include <memory>
#include <string>

namespace artery
{

class FsmDccEntity : public DccEntityBase
{
public:
    void finish() override;
    vanetza::dcc::TransmitRateThrottle* getTransmitRateThrottle() override { return mTransmitRateControl.get(); }

protected:
    void initializeTransmitRateControl() override;
    vanetza::dcc::TransmitRateControl* getTransmitRateControl() override { return mTransmitRateControl.get(); }
    void onGlobalCbr(vanetza::dcc::ChannelLoad) override;

private:
    void initializeStateMachine(const std::string&);
    void initializeTransmitRateControl(const std::string&);

    std::unique_ptr<vanetza::dcc::StateMachine> mStateMachine;
    std::unique_ptr<vanetza::dcc::TransmitRateControl> mTransmitRateControl;
};

} // namespace artery

#endif /* ARTERY_FSMDCCENTITY_H_FY8QUCBE */

