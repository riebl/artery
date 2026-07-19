#pragma once

#include "SionnaRadioDeviceBase.h"

#include <artery/inet/VanetReceiver.h>

#include <optional>

namespace artery::sionna
{

class SionnaVanetReceiver : public artery::VanetReceiver, public SionnaRadioDeviceBase
{
public:
    void finish() override;

    py::Receiver& device();
    const py::Receiver& device() const;

protected:
    void initialize(int stage) override;
    void updatePhysics() override;
    void bindIntoScene() override;

private:
    std::optional<py::Receiver> device_;
};

}  // namespace artery::sionna
