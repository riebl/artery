#pragma once

#include <inet/physicallayer/analogmodel/packetlevel/ScalarAnalogModel.h>

namespace artery::sionna
{

class SionnaScalarAnalogModel : public inet::physicallayer::ScalarAnalogModel
{
public:
    inet::W computeReceptionPower(
        const inet::physicallayer::IRadio* receiverRadio, const inet::physicallayer::ITransmission* transmission,
        const inet::physicallayer::IArrival* arrival) const override;
};

}  // namespace artery::sionna
