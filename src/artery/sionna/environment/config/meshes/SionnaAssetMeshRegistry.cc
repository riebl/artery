// for ref.
// clang-format off
#include <nanobind/nanobind.h>
// clang-format on

#include "SionnaAssetMeshRegistry.h"

#include <artery/sionna/bridge/Fwd.h>
#include <artery/sionna/bridge/bindings/Material.h>
#include <artery/sionna/bridge/bindings/Modules.h>
#include <artery/sionna/environment/config/meshes/IMeshRegistry.h>
#include <mitsuba/core/plugin.h>
#include <mitsuba/core/properties.h>
#include <mitsuba/render/mesh.h>
#include <omnetpp/cexception.h>

using namespace artery::sionna;

Define_Module(SionnaDirectAssetMeshRegistry);

namespace
{

// Even worse than meshes. Taken from itu.py file.
namespace known
{
constexpr const char* concrete = "concrete";
constexpr const char* brick = "brick";
constexpr const char* plasterboard = "plasterboard";
constexpr const char* wood = "wood";
constexpr const char* glass = "glass";
constexpr const char* ceilingBoard = "ceiling_board";
constexpr const char* chipboard = "chipboard";
constexpr const char* plywood = "plywood";
constexpr const char* marble = "marble";
constexpr const char* floorboard = "floorboard";
constexpr const char* metal = "metal";
constexpr const char* veryDryGround = "very_dry_ground";
constexpr const char* mediumDryGround = "medium_dry_ground";
constexpr const char* wetGround = "wet_ground";
}  // namespace known

using path = mitsuba::fs::path;

// I don't think there is a better way to do that.
// Paths are relative to scenes/ module.
const std::unordered_map<MeshAsset, mitsuba::fs::path> meshes = {{MeshAsset::LowPolyCar, path("low_poly_car.ply")}};

const std::unordered_map<MeshAsset, std::string> materials = {{MeshAsset::LowPolyCar, known::metal}};

}  // namespace

void SionnaDirectAssetMeshRegistry::initialize()
{
    root_ = ScenesFileview::scenes();
    EV_INFO << "Using meshes root: " << root_ << "\n";

    if (root_.empty()) {
        throw omnetpp::cRuntimeError("could not find Sionna's scene root: path is empty");
    }
}

mitsuba::ref<mi::Mesh> SionnaDirectAssetMeshRegistry::asset(MeshAsset asset) const
{
    if (auto mesh = meshes.find(asset); mesh == meshes.end()) {
        throw omnetpp::cRuntimeError("could not access mesh: path for this asset is not defined");
    } else {
        const auto& [_, suffix] = *mesh;

        mitsuba::Properties props("ply");
        props.set("filename", (root_ / suffix).string());

        return mitsuba::PluginManager::instance()->create_object<mi::Mesh>(props);
    }
}

py::RadioMaterial SionnaDirectAssetMeshRegistry::material(MeshAsset asset) const
{
    if (auto cached = cachedMaterials_.find(asset); cached != cachedMaterials_.end()) {
        return cached->second;
    }

    if (auto material = materials.find(asset); material == materials.end()) {
        throw omnetpp::cRuntimeError("could not access material: material for this asset is not defined");
    } else {
        const auto& [_, type] = *material;

        mitsuba::Properties props("itu-radio-material");
        props.set("type", type);
        props.set_id(format("mat-%s", type.c_str()));

        auto object = mitsuba::PluginManager::instance()->create_object(props, VariantName::name, mitsuba::ObjectType::BSDF);

        auto [it, _inserted] = cachedMaterials_.emplace(asset, py::RadioMaterial(nanobind::cast(object)));
        return it->second;
    }
}

mi::Vector3f SionnaDirectAssetMeshRegistry::scaling(MeshAsset asset) const
{
    // Do not cache the vectors, they may be only allowed to be created after proper setup.
    switch (asset) {
        case MeshAsset::LowPolyCar:
            return {0.5, 0.5, 0.5};
    }

    throw omnetpp::cRuntimeError("could not access scaling: scaling for this asset is not defined");
}
