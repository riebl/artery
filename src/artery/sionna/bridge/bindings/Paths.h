#pragma once

// for ref.
// clang-format off
#include <nanobind/nanobind.h>
// clang-format on

#include <artery/sionna/bridge/Fwd.h>
#include <artery/sionna/bridge/bindings/AntennaArray.h>
#include <artery/sionna/bridge/bindings/Modules.h>
#include <artery/sionna/bridge/capabilities/Calling.h>
#include <drjit/tensor.h>

namespace artery::sionna::py
{

class SIONNA_BRIDGE_API Paths : public SionnaRtModule, public InitPythonClassCapability
{
public:
    // IPythonClassIdentityCapability implementation.
    const char* className() const override;

    // Default constructor.
    Paths() = default;

    // Wrap an existing Sionna Paths python object returned by PathSolver.
    explicit Paths(nanobind::object obj);

    // Number of transmitters and receivers included in this solved paths object.
    // Indices passed to pathGain(rxIndex, txIndex) must be within these counts.
    std::size_t numTx() const;
    std::size_t numRx() const;

    // Whether Sionna solved paths using the synthetic-array approximation.
    // When true, array effects are applied synthetically instead of tracing
    // separate paths for every antenna element.
    bool syntheticArray() const;

    // Antenna array descriptions used for the solved transmitter and receiver devices.
    AntennaArray txArray() const;
    AntennaArray rxArray() const;

    // Path validity mask returned by Sionna. Its shape follows Sionna's Paths.valid
    // tensor layout and marks which candidate paths carry usable propagation data.
    mi::TensorXf::MaskType valid() const;

    // Complex path coefficients returned as real and imaginary TensorXf parts.
    std::tuple<mitsuba::Resolve::TensorXf, mitsuba::Resolve::TensorXf> coefficients() const;

    // Total linear path gain across all receivers, transmitters, antenna dimensions, and valid paths.
    double pathGain() const;

    // Linear path gain for one receiver/transmitter pair, summed over remaining
    // antenna/path dimensions. rxIndex and txIndex use Sionna's receiver/transmitter order.
    double pathGain(std::size_t rxIndex, std::size_t txIndex) const;
};

}  // namespace artery::sionna::py
