#pragma once

// for ref.
// clang-format off
#include <nanobind/nanobind.h>
// clang-format on

#include <artery/sionna/bridge/Compat.h>
#include <artery/sionna/bridge/bindings/Constants.h>
#include <artery/sionna/bridge/capabilities/Calling.h>
#include <artery/sionna/bridge/capabilities/Core.h>
#include <drjit/array_traits.h>

#include <string>
#include <tuple>

namespace artery::sionna::py
{

class SIONNA_BRIDGE_API RadioMaterialBase : public SionnaRtModule, public InitPythonClassCapability
{
public:
    // RGB color used by Sionna/Mitsuba for visualization. This is separate
    // from the radio propagation parameters below.
    using TColor = std::tuple<float, float, float>;

    // IPythonClassIdentityCapability implementation.
    const char* className() const override;

    // Returns material name.
    std::string materialName() const;

    // Access material visualization color.
    TColor color() const;
    void setColor(TColor newColor);
};

// Wrapper around sionna.rt.RadioMaterial. RadioMaterial is the material type
// expected by Sionna path solvers for scene objects.
class SIONNA_BRIDGE_API RadioMaterial : public DefaultedClassProviderCapability, public RadioMaterialBase
{
public:
    // IPythonClassIdentityCapability implementation.
    const char* className() const override;

    // Construct Sionna's default radio material.
    RadioMaterial();

    // Wrap an existing Sionna RadioMaterial python object.
    explicit RadioMaterial(nanobind::object obj);

    // Construct a named material from radio parameters:
    // conductivity [S/m], relativePermittivity [-], and thickness [m].
    RadioMaterial(
        const std::string& name, mi::Float64 conductivity = 0.0, mi::Float64 relativePermittivity = 1.0,
        typename Defaulted<mi::Float64>::Argument thickness = Constants::defaultThickness);

    // Relative permittivity controls dielectric response in propagation.
    maybe_diff_t<mi::Float> relativePermittivity() const;
    void setRelativePermittivity(maybe_diff_t<mi::Float> relativePermittivity);

    // Electrical conductivity in S/m.
    maybe_diff_t<mi::Float> conductivity() const;
    void setConductivity(maybe_diff_t<mi::Float> conductivity);

    // Effective material thickness in meters, used by Sionna penetration/refraction logic.
    maybe_diff_t<mi::Float> thickness() const;
    void setThickness(maybe_diff_t<mi::Float> thickness);
};

}  // namespace artery::sionna::py
