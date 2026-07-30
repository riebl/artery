#pragma once

#include <artery/sionna/bridge/SionnaBridge.h>
#include <inet/common/INETDefs.h>
#include <inet/physicallayer/base/packetlevel/AntennaBase.h>
#include <omnetpp/cmodule.h>

#include <optional>

namespace artery::sionna
{

enum class SceneArrayRole { None, Tx, Rx };

class SionnaAntennaArray : public inet::physicallayer::AntennaBase
{
public:
    SionnaAntennaArray() = default;

    // omnetpp::cSimpleModule implementation.
    int numInitStages() const override;
    void initialize(int stage) override;

    // inet::physicallayer::IAntenna implementation.
    std::ostream& printToStream(std::ostream& stream, int level) const override;
    double getMaxGain() const override;
    double computeGain(const inet::EulerAngles direction) const override;

    // Sionna antenna array access.
    const py::AntennaArray& array() const;

protected:
    void setArray(py::AntennaArray array);
    SceneArrayRole sceneArrayRole() const;

protected:
    std::optional<py::AntennaArray> array_;
    double gain_ = 1.0;
};

}  // namespace artery::sionna
