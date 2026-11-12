#pragma once

#include <artery/sionna/bridge/Fwd.h>
#include <artery/sionna/bridge/Helpers.h>
#include <artery/sionna/bridge/bindings/Material.h>
#include <artery/sionna/bridge/bindings/Modules.h>
#include <artery/sionna/bridge/capabilities/Core.h>
#include <mitsuba/core/filesystem.h>
#include <mitsuba/render/mesh.h>
#include <nanobind/stl/string.h>

#include <cstdint>

namespace artery::sionna
{

// Common entry point for Sionna-provided meshes and other scene stuff.
class ScenesFileview
{
public:
    // Returns path to scenes root. All assets should be relative to this path.
    static mitsuba::fs::path scenes()
    {
        class Importer : public py::SionnaRtScenesModule, public py::CachedImportCapability
        {
        } importer;

        auto module = importer.module();
        auto path = sionna::access<std::string>(module, "__file__");

        return mitsuba::fs::path(path).parent_path();
    }
};

// Logical keys for mesh assets used by the simulation.
enum class MeshAsset : std::uint8_t { LowPolyCar };

// Registry contract for retrieving Mitsuba meshes by logical asset id.
class IMeshRegistry
{
public:
    virtual ~IMeshRegistry() = default;

    // Return mesh for a given logical asset.
    virtual mitsuba::ref<mi::Mesh> asset(MeshAsset asset) const = 0;

    // Return material for a given logical asset.
    virtual py::RadioMaterial material(MeshAsset asset) const = 0;

    // Return scaling vector for asset.
    virtual mi::Vector3f scaling(MeshAsset asset) const = 0;
};

}  // namespace artery::sionna
