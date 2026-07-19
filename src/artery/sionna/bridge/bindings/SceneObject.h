#pragma once

// for ref.
// clang-format off
#include <nanobind/nanobind.h>
// clang-format on

#include <artery/sionna/bridge/bindings/Material.h>
#include <artery/sionna/bridge/bindings/Modules.h>
#include <artery/sionna/bridge/capabilities/Calling.h>
#include <artery/sionna/bridge/capabilities/Core.h>
#include <mitsuba/core/object.h>

#include <string>

namespace artery::sionna::py
{

// Scene Objects present in the Sionna Scene. Sionna aggregates
// already present shapes in initial mitsuba scene, then creates this
// wrapper for them. You may add objects or access their properties via
// this class. Upon scene reconstruction, these objects stay valid.
class SIONNA_BRIDGE_API SceneObject : public SionnaRtModule, public InitPythonClassCapability
{
public:
    // IPythonClassIdentityCapability implementation.
    const char* className() const override;

    // Default constructor, does not hold valid object yet.
    SceneObject() = default;

    // Assume passed object is of type SceneObject.
    explicit SceneObject(nanobind::object obj);

    // Initialize scene object from mesh and material, assuming all mesh consists of the same material.
    SceneObject(mitsuba::ref<mi::Mesh> mesh, const RadioMaterial& material);
    // Same as above, but also assigns custom ID.
    SceneObject(mitsuba::ref<mi::Mesh> mesh, const std::string& name, const RadioMaterial& material);
    // Almost like above, just uses mesh name (loads from file) to initialize mesh.
    SceneObject(const std::string& fname, const std::string& name, const RadioMaterial& material);

    // Returns ID (name) for object.
    std::string name() const;
    // Returns position with three components.
    mi::Point3f position() const;
    // Returns orientation, which defines rotation for this object.
    mi::Point3f orientation() const;
    // Returns scaling for mesh alongside all components.
    mi::Vector3f scaling() const;
    // Returns velocity for this object, used for computation of path loss.
    mi::Vector3f velocity() const;
    // Returns bound mi.Mesh.
    mitsuba::ref<mi::Mesh> mesh() const;
    // Returns material for this object.
    RadioMaterial material() const;

    // Sets position in scene for this object. Valid only if object is already added to the scene.
    void setPosition(const mi::Point3f& position);
    // Sets orientation in scene for this object. Valid only if object is already added to the scene.
    void setOrientation(const mi::Point3f& orientation);
    // Sets scaling for all components.
    void setScaling(const mi::Vector3f& scaling);
    // Sets velocity. Only relevant for path loss, object is not physically moving.
    void setVelocity(const mi::Vector3f& velocity);
    // Sets material. Note, they also define colors for objects upon render.
    void setMaterial(const RadioMaterial& material);
};

}  // namespace artery::sionna::py
