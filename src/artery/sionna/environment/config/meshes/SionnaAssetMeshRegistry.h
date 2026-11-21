#pragma once

#include <artery/sionna/bridge/bindings/Modules.h>
#include <artery/sionna/environment/config/meshes/IMeshRegistry.h>
#include <mitsuba/core/filesystem.h>
#include <omnetpp/csimplemodule.h>

#include <unordered_map>

namespace artery::sionna
{

// Simple mesh registry that returns meshes from static mapping.
class SionnaDirectAssetMeshRegistry : public IMeshRegistry, public omnetpp::cSimpleModule
{
public:
    SionnaDirectAssetMeshRegistry() = default;

    // omnetpp::cSimpleModule implementation.
    void initialize() override;

    // IMeshRegistry implementation.
    mitsuba::ref<mi::Mesh> asset(MeshAsset asset) const override;
    py::RadioMaterial material(MeshAsset asset) const override;
    mi::Vector3f scaling(MeshAsset asset) const override;

private:
    mitsuba::fs::path root_;
    mutable std::unordered_map<MeshAsset, py::RadioMaterial> cachedMaterials_;
};

}  // namespace artery::sionna
