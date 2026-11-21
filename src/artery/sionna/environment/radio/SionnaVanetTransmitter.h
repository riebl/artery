#pragma once

#include "SionnaRadioDeviceBase.h"

#include <inet/physicallayer/ieee80211/packetlevel/Ieee80211ScalarTransmitter.h>

#include <optional>

namespace artery::sionna
{

class SionnaVanetTransmitter : public inet::physicallayer::Ieee80211ScalarTransmitter, public SionnaRadioDeviceBase
{
public:
    void finish() override;

    py::Transmitter& device();
    const py::Transmitter& device() const;

    void setPowerDbm(float powerDbm);

protected:
    void initialize(int stage) override;
    void updatePhysics() override;
    void bindIntoScene() override;

private:
    std::optional<py::Transmitter> device_;
};

}  // namespace artery::sionna
