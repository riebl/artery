#include "Material.h"

#include <artery/sionna/bridge/Helpers.h>

using namespace artery::sionna;
using namespace artery::sionna::literals;

const char* py::RadioMaterialBase::className() const
{
    return "RadioMaterialBase";
}

std::string py::RadioMaterialBase::materialName() const
{
    return sionna::access<std::string>(this->bound_, "name");
}

auto py::RadioMaterialBase::color() const -> TColor
{
    return sionna::access<TColor>(this->bound_, "color");
}

void py::RadioMaterialBase::setColor(TColor newColor)
{
    sionna::set<TColor>(this->bound_, "color", std::move(newColor));
}

const char* py::RadioMaterial::className() const
{
    return "RadioMaterial";
}

py::RadioMaterial::RadioMaterial() = default;

py::RadioMaterial::RadioMaterial(nanobind::object obj)
{
    this->init(std::move(obj));
}

py::RadioMaterial::RadioMaterial(
    const std::string& name, mi::Float64 conductivity, mi::Float64 relativePermittivity, typename Defaulted<mi::Float64>::Argument thickness)
{
    this->InitPythonClassCapability::init(
        "name"_a = name, "thickness"_a = std::move(thickness), "relative_permittivity"_a = relativePermittivity, "conductivity"_a = conductivity);
}

maybe_diff_t<mi::Float> py::RadioMaterial::relativePermittivity() const
{
    return sionna::access<maybe_diff_t<mi::Float>>(this->bound_, "relative_permittivity");
}

void py::RadioMaterial::setRelativePermittivity(maybe_diff_t<mi::Float> relativePermittivity)
{
    sionna::set(this->bound_, "relative_permittivity", relativePermittivity);
}

maybe_diff_t<mi::Float> py::RadioMaterial::conductivity() const
{
    return sionna::access<maybe_diff_t<mi::Float>>(this->bound_, "conductivity");
}

void py::RadioMaterial::setConductivity(maybe_diff_t<mi::Float> conductivity)
{
    sionna::set(this->bound_, "conductivity", conductivity);
}

maybe_diff_t<mi::Float> py::RadioMaterial::thickness() const
{
    return sionna::access<maybe_diff_t<mi::Float>>(this->bound_, "thickness");
}

void py::RadioMaterial::setThickness(maybe_diff_t<mi::Float> thickness)
{
    sionna::set(this->bound_, "thickness", thickness);
}
