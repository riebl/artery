#include "PathSolver.h"

#include <artery/sionna/bridge/Helpers.h>

using namespace artery::sionna;
using namespace artery::sionna::literals;

const char* py::PathSolver::className() const
{
    return "PathSolver";
}

py::PathSolver::PathSolver()
{
    InitPythonClassCapability::init();
}

py::PathSolver::PathSolver(nanobind::object obj)
{
    WrapPythonClassCapability::init(std::move(obj));
}

py::Paths py::PathSolver::solve(
    const SionnaScene& scene, int maxDepth, int maxNumPathsPerSrc, int samplesPerSrc, bool syntheticArray, bool los, bool specularReflection,
    bool diffuseReflection, bool refraction, bool diffraction, bool edgeDiffraction, bool diffractionLitRegion, int seed) const
{
    return py::Paths(
        sionna::call<nanobind::object>(
            bound_, "__call__", "scene"_a = scene, "max_depth"_a = maxDepth, "max_num_paths_per_src"_a = maxNumPathsPerSrc, "samples_per_src"_a = samplesPerSrc,
            "synthetic_array"_a = syntheticArray, "los"_a = los, "specular_reflection"_a = specularReflection, "diffuse_reflection"_a = diffuseReflection,
            "refraction"_a = refraction, "diffraction"_a = diffraction, "edge_diffraction"_a = edgeDiffraction,
            "diffraction_lit_region"_a = diffractionLitRegion, "seed"_a = seed));
}
