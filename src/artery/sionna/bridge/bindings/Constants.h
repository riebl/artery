#pragma once

#include <artery/sionna/bridge/Defaulted.h>
#include <artery/sionna/bridge/bindings/Modules.h>
#include <artery/sionna/bridge/capabilities/Defaulted.h>
#include <mitsuba/core/fwd.h>

namespace artery::sionna::py
{

// Accessor for constants exported by Sionna. Values are resolved lazily so
// they remain valid after the Python runtime and Mitsuba variant are initialized.
class SIONNA_BRIDGE_API Constants : public SionnaRtModule, public DefaultedModuleProviderCapability
{
public:
    // IPythonModuleIdentityCapability implementation.
    const char* moduleName() const override;

    // Sionna's default effective material thickness, used when constructing
    // radio materials without an explicit thickness.
    static const DefaultedWithDeferredResolution<mi::Float64, ModuleResolver<Constants>> defaultThickness;
};

// Wrapper for Sionna interaction type constants used by Paths.interactions.
// These classify what happened at each path vertex.
class SIONNA_BRIDGE_API IntersectionTypes : public Constants, public DefaultedClassProviderCapability
{
public:
    // Deferred wrapper for one interaction-type value.
    template <typename T>
    using TIntersection = DefaultedWithDeferredResolution<T, ModuleAndClassResolver<IntersectionTypes>>;

    // IPythonClassIdentityCapability implementation.
    const char* className() const override;

    // No interaction at this depth, usually terminates the path.
    static const TIntersection<mi::Int32> none;

    // Specular reflection interaction.
    static const TIntersection<mi::Int32> specular;

    // Diffuse reflection interaction.
    static const TIntersection<mi::Int32> diffuse;

    // Transmission/refraction through a material.
    static const TIntersection<mi::Int32> refraction;

    // Diffraction interaction around an edge.
    static const TIntersection<mi::Int32> diffraction;
};

}  // namespace artery::sionna::py
