#ifndef ARTERY_LIMERICDCCENTITY_H_JIVG5BNY
#define ARTERY_LIMERICDCCENTITY_H_JIVG5BNY

#include "artery/networking/DccEntityBase.h"
#include <vanetza/dcc/limeric.hpp>
#include <vanetza/dcc/limeric_transmit_rate_control.hpp>
#include <memory>

namespace artery
{

class LimericDccEntity : public DccEntityBase
{
public:
    void finish() override;
    vanetza::dcc::TransmitRateThrottle* getTransmitRateThrottle() override;

protected:
    void initializeTransmitRateControl() override;
    vanetza::dcc::TransmitRateControl* getTransmitRateControl() override;
    void onGlobalCbr(vanetza::dcc::ChannelLoad) override;

private:
    std::unique_ptr<vanetza::dcc::Limeric> mAlgorithm;
    std::unique_ptr<vanetza::dcc::LimericTransmitRateControl> mTransmitRateControl;
};

} // namespace artery

#endif /* ARTERY_LIMERICDCCENTITY_H_JIVG5BNY */

