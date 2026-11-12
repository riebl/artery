#pragma once

#include "ISionnaActor.h"

#include <artery/sionna/bridge/bindings/SceneObject.h>
#include <artery/sionna/environment/api/SionnaAPI.h>
#include <artery/sionna/environment/config/meshes/IMeshRegistry.h>
#include <traci/Angle.h>
#include <traci/Position.h>

namespace artery::sionna
{

class SionnaActorBase : public virtual ISionnaActor
{
public:
    // ISionnaActor implementation.
    mi::Point3f position() const override;
    mi::Point3f orientation() const override;
    mi::Vector3f velocity() const override;
    mitsuba::ref<mi::Mesh> mesh() const override;

    // Returns ID in Sionna's scene, derived from the one in SUMO.
    std::string sceneId(const std::string& sumoId) const;

    // Access bound mesh registry.
    IMeshRegistry* meshRegistry();
    // Access bound mesh registry.
    const IMeshRegistry* meshRegistry() const;
    // Access bound API.
    ISionnaAPI* api();
    // Access bound API.
    const ISionnaAPI* api() const;
    // Access bound scene actor's object.
    py::SceneObject& object();
    // Access bound scene actor's object.
    const py::SceneObject& object() const;

protected:
    // Initialize the base.
    void initializeSionnaActor(omnetpp::cSimpleModule* owner);
    // Creates actor's scene object. Override in derived classes for actor-specific assets.
    virtual py::SceneObject makeSceneObject(const std::string& sumoId) const;
    // Updates actor from TraCI data.
    virtual void updateSionnaObject(
        IDynamicSceneConfigProxy::ITransformProxy* proxy, const libsumo::TraCIPosition& TraCIPosition, traci::TraCIAngle heading, bool adjust = true);
    // Removes actor by SUMO ID.
    virtual void removeSionnaObject(const std::string& sumoId);

private:
    ISionnaAPI* api_ = nullptr;
    IMeshRegistry* meshRegistry_ = nullptr;
    py::SceneObject object_;
};

}  // namespace artery::sionna
