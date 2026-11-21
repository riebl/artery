#include "SionnaActorBase.h"

#include <artery/sionna/environment/Compat.h>
#include <inet/common/ModuleAccess.h>

using namespace artery::sionna;

mi::Point3f SionnaActorBase::position() const
{
    return api_->coordinateTransform()->fromSionnaScene(object_.position());
}

mi::Point3f SionnaActorBase::orientation() const
{
    return api_->coordinateTransform()->fromSionnaScene(object_.orientation());
}

mi::Vector3f SionnaActorBase::velocity() const
{
    return api_->coordinateTransform()->fromSionnaScene(object_.velocity());
}

mitsuba::ref<mi::Mesh> SionnaActorBase::mesh() const
{
    return object_.mesh();
}

void SionnaActorBase::initializeSionnaActor(omnetpp::cSimpleModule* owner)
{
    api_ = ISionnaAPI::get(owner);
    meshRegistry_ = inet::getModuleFromPar<IMeshRegistry>(owner->par("meshRegistryModule"), owner, true);
}

IMeshRegistry* SionnaActorBase::meshRegistry()
{
    return meshRegistry_;
}

const IMeshRegistry* SionnaActorBase::meshRegistry() const
{
    return meshRegistry_;
}

ISionnaAPI* SionnaActorBase::api()
{
    return api_;
}

const ISionnaAPI* SionnaActorBase::api() const
{
    return api_;
}

py::SceneObject& SionnaActorBase::object()
{
    return object_;
}

const py::SceneObject& SionnaActorBase::object() const
{
    return object_;
}

std::string SionnaActorBase::sceneId(const std::string& sumoId) const
{
    return api_->IDConversion()->convertID(IDNamespace::SUMO, IDNamespace::SIONNA, sumoId);
}

py::SceneObject SionnaActorBase::makeSceneObject(const std::string& sumoId) const
{
    // NOTE: this is so wrong, ideally this should be virtual and this
    // kind of configuration happens in children. There is nothing except
    // for cars now, so leave as-is until some more meshes become available.
    const auto mesh = meshRegistry()->asset(MeshAsset::LowPolyCar);
    const auto material = meshRegistry()->material(MeshAsset::LowPolyCar);
    return py::SceneObject(mesh, sceneId(sumoId), material);
}

void SionnaActorBase::updateSionnaObject(
    IDynamicSceneConfigProxy::ITransformProxy* proxy, const libsumo::TraCIPosition& TraCIPosition, traci::TraCIAngle heading, bool adjust)
{
    auto* transform = api_->coordinateTransform();

    const auto position = transform->fromSumo(TraCIPosition);
    const auto orientation = transform->fromSumo(heading);
    const auto scaling = meshRegistry()->scaling(MeshAsset::LowPolyCar);

    proxy->transform([this, transform, position, orientation, scaling, adjust](py::SceneObject& object) {
        object.setOrientation(orientation);
        object.setScaling(scaling);

        auto pos = transform->fromSionnaScene(position);
        if (adjust) {
            pos.z() = transform->fromSionnaScene(object.position()).z();
        } else {
            // NOTE: temp solution to actually get first valid Z along the axis.
            // should be patched in a more robust approach, like taking max coord of the scene.
            pos.z() = 1000;
        }

        object.setPosition(transform->toSionnaScene(pos));
        transform->adjustVerticalComponent(object);
    });
}

void SionnaActorBase::removeSionnaObject(const std::string& sumoId)
{
    api_->dynamicConfiguration()->removeSceneItem(sceneId(sumoId));
    api_->IDConversion()->removeID(IDNamespace::SUMO, sumoId);
}
