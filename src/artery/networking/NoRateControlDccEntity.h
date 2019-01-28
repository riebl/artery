#ifndef ARTERY_NORATECONTROLDCCENTITY_H_FC9BW4JV
#define ARTERY_NORATECONTROLDCCENTITY_H_FC9BW4JV

#include "artery/networking/DccEntityBase.h"

namespace artery
{

class NoRateControlDccEntity : public DccEntityBase
{
public:
    vanetza::dcc::TransmitRateThrottle* getTransmitRateThrottle() override;

protected:
    void initializeTransmitRateControl() override;
    vanetza::dcc::TransmitRateControl* getTransmitRateControl() override;
    void onGlobalCbr(vanetza::dcc::ChannelLoad) override;

private:
    std::unique_ptr<vanetza::dcc::TransmitRateControl> mTransmitRateControl;
};

} // namespace artery


#endif /* ARTERY_NORATECONTROLDCCENTITY_H_FC9BW4JV */

