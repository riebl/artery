#pragma once

// for ref.
// clang-format off
#include <nanobind/nanobind.h>
// clang-format on

#include <artery/sionna/bridge/bindings/Modules.h>
#include <artery/sionna/bridge/capabilities/Calling.h>

#include <optional>
#include <string>

namespace artery::sionna::py
{

// Wrapper around Sionna antenna array objects. These describe antenna
// geometry/patterns used by transmitters, receivers, and solved Paths.
class SIONNA_BRIDGE_API AntennaArray : public SionnaRtModule, public InitPythonClassCapability
{
public:
    const char* className() const override;

    AntennaArray() = default;

    // Wrap an existing Sionna antenna array python object.
    explicit AntennaArray(nanobind::object obj);

    // Total number of antenna elements in the array.
    std::size_t numAntennas() const;

    // Sionna array_size property, used when interpreting Paths tensor dimensions.
    std::size_t arraySize() const;
};

// Wrapper around sionna.rt.PlanarArray.
class SIONNA_BRIDGE_API PlanarArray : public AntennaArray
{
public:
    const char* className() const override;

    PlanarArray() = default;

    // Wrap an existing Sionna PlanarArray python object.
    explicit PlanarArray(nanobind::object obj);

    // Construct a rectangular planar array. Spacing values are in wavelength units,
    // matching Sionna's PlanarArray API.
    PlanarArray(
        int numRows, int numCols, const std::string& pattern, float verticalSpacing = 0.5f, float horizontalSpacing = 0.5f,
        const std::string& polarization = "V", const std::string& polarizationModel = "tr38901_2");
};

}  // namespace artery::sionna::py
