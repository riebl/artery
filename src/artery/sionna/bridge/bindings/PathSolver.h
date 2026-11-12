#pragma once

// for ref.
// clang-format off
#include <nanobind/nanobind.h>
// clang-format on

#include <artery/sionna/bridge/bindings/Modules.h>
#include <artery/sionna/bridge/bindings/Paths.h>
#include <artery/sionna/bridge/bindings/Scene.h>
#include <artery/sionna/bridge/capabilities/Calling.h>

namespace artery::sionna::py
{

// Sionna path solver. These solvers generate paths objects containing calculated
// signal paths, which may be visualized on render() call.
class SIONNA_BRIDGE_API PathSolver : public SionnaRtModule, public InitPythonClassCapability
{
public:
    // IPythonClassIdentityCapability implementation.
    const char* className() const override;

    // Default constructor.
    PathSolver();
    // Wrap existing object, assuming it's PathSolver.
    explicit PathSolver(nanobind::object obj);

    // Calculate propagation paths for the current scene. For full doc, please
    // refer to Sionna's PathSolver call, but basically:
    // 1) scene must contain bound transmitters, receivers, scene objects, and radio materials
    // 2) maxDepth limits the number of interactions along each path
    // 3) maxNumPathsPerSrc and samplesPerSrc control search budget and runtime
    // 4) syntheticArray toggles Sionna's synthetic-array approximation
    // 5) los, specularReflection, diffuseReflection, refraction, diffraction, edgeDiffraction,
    // and diffractionLitRegion select which propagation mechanisms are considered
    // 6) seed makes the stochastic path search reproducible
    Paths solve(
        const SionnaScene& scene, int maxDepth = 3, int maxNumPathsPerSrc = 1000000, int samplesPerSrc = 1000000, bool syntheticArray = true, bool los = true,
        bool specularReflection = true, bool diffuseReflection = false, bool refraction = true, bool diffraction = false, bool edgeDiffraction = false,
        bool diffractionLitRegion = true, int seed = 42) const;
};

}  // namespace artery::sionna::py
