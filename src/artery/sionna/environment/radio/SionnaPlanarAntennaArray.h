#pragma once

#include "SionnaAntennaArray.h"

namespace artery::sionna
{

class SionnaPlanarAntennaArray : public SionnaAntennaArray
{
public:
    SionnaPlanarAntennaArray() = default;

    // omnetpp::cSimpleModule implementation.
    void initialize(int stage) override;

    // inet::physicallayer::IAntenna implementation.
    std::ostream& printToStream(std::ostream& stream, int level) const override;
};

}  // namespace artery::sionna
