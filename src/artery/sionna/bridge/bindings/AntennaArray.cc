#include "AntennaArray.h"

#include <artery/sionna/bridge/Helpers.h>

using namespace artery::sionna;
using namespace artery::sionna::literals;

const char* py::AntennaArray::className() const
{
    return "AntennaArray";
}

py::AntennaArray::AntennaArray(nanobind::object obj)
{
    WrapPythonClassCapability::init(std::move(obj));
}

std::size_t py::AntennaArray::numAntennas() const
{
    return sionna::access<std::size_t>(bound_, "num_ant");
}

std::size_t py::AntennaArray::arraySize() const
{
    return sionna::access<std::size_t>(bound_, "array_size");
}

const char* py::PlanarArray::className() const
{
    return "PlanarArray";
}

py::PlanarArray::PlanarArray(nanobind::object obj)
{
    WrapPythonClassCapability::init(std::move(obj));
}

py::PlanarArray::PlanarArray(
    int numRows, int numCols, const std::string& pattern, float verticalSpacing, float horizontalSpacing, const std::string& polarization,
    const std::string& polarizationModel)
{
    InitPythonClassCapability::init(
        "num_rows"_a = numRows, "num_cols"_a = numCols, "pattern"_a = pattern, "vertical_spacing"_a = verticalSpacing,
        "horizontal_spacing"_a = horizontalSpacing, "polarization"_a = polarization, "polarization_model"_a = polarizationModel);
}
