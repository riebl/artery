#ifndef ARTERY_NORATECONTROLDCCENTITY_CC_LKWSFFXB
#define ARTERY_NORATECONTROLDCCENTITY_CC_LKWSFFXB

#include "artery/networking/NoRateControlDccEntity.h"
#include "artery/utility/PointerCheck.h"

namespace artery
{

Define_Module(NoRateControlDccEntity)

namespace
{

class NoopTransmitRateControl : public vanetza::dcc::TransmitRateControl
{
public:
    vanetza::Clock::duration delay(const vanetza::dcc::Transmission&) override
    {
        return vanetza::Clock::duration::zero();
    }

    vanetza::Clock::duration interval(const vanetza::dcc::Transmission&) override
    {
        return vanetza::Clock::duration::zero();
    }

    void notify(const vanetza::dcc::Transmission& tx) override
    {
    }
};

}

vanetza::dcc::TransmitRateThrottle* NoRateControlDccEntity::getTransmitRateThrottle()
{
    return notNullPtr(mTransmitRateControl);
}

vanetza::dcc::TransmitRateControl* NoRateControlDccEntity::getTransmitRateControl()
{
    return notNullPtr(mTransmitRateControl);
}

void NoRateControlDccEntity::initializeTransmitRateControl()
{
    mTransmitRateControl.reset(new NoopTransmitRateControl());
}

void NoRateControlDccEntity::onGlobalCbr(vanetza::dcc::ChannelLoad)
{
}

} // namespace artery


#endif /* ARTERY_NORATECONTROLDCCENTITY_CC_LKWSFFXB */

