#include "SionnaAntennaArray.h"

#include <artery/sionna/environment/api/SionnaAPI.h>
#include <inet/common/INETMath.h>
#include <inet/common/InitStages.h>
#include <omnetpp/cexception.h>

using namespace artery::sionna;

Define_Module(SionnaAntennaArray);

int SionnaAntennaArray::numInitStages() const
{
    return inet::NUM_INIT_STAGES;
}

void SionnaAntennaArray::initialize(int stage)
{
    inet::physicallayer::AntennaBase::initialize(stage);
    if (stage == inet::INITSTAGE_LOCAL) {
        gain_ = inet::math::dB2fraction(par("gain"));
    } else if (stage == inet::INITSTAGE_LAST) {
        if (sceneArrayRole() == SceneArrayRole::None) {
            return;
        }

        auto* api = ISionnaAPI::get(this);
        switch (sceneArrayRole()) {
            case SceneArrayRole::None:
                return;
            case SceneArrayRole::Tx:
                if (!api->setTxArray(array())) {
                    throw omnetpp::cRuntimeError("Sionna scene already has tx_array configured");
                }
                break;
            case SceneArrayRole::Rx:
                if (!api->setRxArray(array())) {
                    throw omnetpp::cRuntimeError("Sionna scene already has rx_array configured");
                }
                break;
        }
    }
}

std::ostream& SionnaAntennaArray::printToStream(std::ostream& stream, int level) const
{
    stream << "SionnaAntennaArray";
    if (level <= PRINT_LEVEL_DETAIL) {
        stream << ", gain = " << gain_ << ", numAntennas = " << numAntennas;
    }
    return inet::physicallayer::AntennaBase::printToStream(stream, level);
}

double SionnaAntennaArray::getMaxGain() const
{
    return gain_;
}

double SionnaAntennaArray::computeGain(const inet::EulerAngles /* direction */) const
{
    return gain_;
}

const py::AntennaArray& SionnaAntennaArray::array() const
{
    if (!array_.has_value()) {
        throw omnetpp::cRuntimeError("SionnaAntennaArray has no bound Sionna array");
    }

    return *array_;
}

void SionnaAntennaArray::setArray(py::AntennaArray array)
{
    numAntennas = static_cast<int>(array.numAntennas());
    array_.emplace(std::move(array));
}

SceneArrayRole SionnaAntennaArray::sceneArrayRole() const
{
    auto role = par("sceneArrayRole").stdstringValue();
    if (role.empty()) {
        return SceneArrayRole::None;
    } else if (role == "tx") {
        return SceneArrayRole::Tx;
    } else if (role == "rx") {
        return SceneArrayRole::Rx;
    } else {
        throw omnetpp::cRuntimeError("Unsupported sceneArrayRole '%s'", role.c_str());
    }
}
