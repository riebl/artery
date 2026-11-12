#include "SionnaRadioDeviceBase.h"

#include <inet/common/InitStages.h>
#include <mitsuba/render/mesh.h>
#include <omnetpp/cexception.h>
#include <omnetpp/cmodule.h>

using namespace artery::sionna;

void SionnaRadioDeviceBase::initializeSionnaRadioDevice(omnetpp::cModule* owner)
{
    owner_ = owner;

    if (mobility_ = radio()->getAntenna()->getMobility(); mobility_ == nullptr) {
        auto* module = dynamic_cast<const omnetpp::cModule*>(radio());
        throw omnetpp::cRuntimeError("Radio %s has no antenna mobility", module ? module->getFullPath().c_str() : "<unknown>");
    }

    if (actor_ = dynamic_cast<ISionnaActor*>(mobility_); actor_ == nullptr) {
        auto* module = dynamic_cast<omnetpp::cModule*>(mobility_);
        throw omnetpp::cRuntimeError("Sionna radio device mobility %s is not a Sionna actor", module ? module->getFullPath().c_str() : "<unknown>");
    }

    bindIntoScene();
    subscribeToDynamicSceneUpdates(owner_);
}

void SionnaRadioDeviceBase::finishSionnaRadioDevice()
{
    unsubscribeFromDynamicSceneUpdates();
    actor_ = nullptr;
    mobility_ = nullptr;
    owner_ = nullptr;
}

void SionnaRadioDeviceBase::onDynamicSceneEdited()
{
    updatePhysics();
}

const inet::physicallayer::IRadio* SionnaRadioDeviceBase::radio() const
{
    if (auto* radio = dynamic_cast<const inet::physicallayer::IRadio*>(owner_->getParentModule()); radio == nullptr) {
        throw omnetpp::cRuntimeError("%s has no bound parent radio", owner_->getClassName());
    } else {
        return radio;
    }
}

inet::IMobility* SionnaRadioDeviceBase::mobility() const
{
    return mobility_;
}

ISionnaAPI* SionnaRadioDeviceBase::api() const
{
    return ISionnaAPI::get(owner_);
}

ISionnaActor* SionnaRadioDeviceBase::actor() const
{
    return actor_;
}

std::string SionnaRadioDeviceBase::sceneID() const
{
    return api()->IDConversion()->convertID(IDNamespace::SUMO, IDNamespace::SIONNA, owner_->getFullPath());
}

mi::Point3f SionnaRadioDeviceBase::position() const
{
    auto* transform = api()->coordinateTransform();
    auto position = actor_->position();

    const auto bbox = actor_->mesh()->bbox();
    const auto min = transform->fromSionnaScene(bbox.min);
    const auto max = transform->fromSionnaScene(bbox.max);

    // NOTE: place device slightly above bbox, so it will not hit vehicle itself on transmission.
    position.z() = drjit::maximum(min.z(), max.z()) + mi::Float(1e-3);

    return transform->toSionnaScene(position);
}

mi::Point3f SionnaRadioDeviceBase::orientation() const
{
    return actor_->orientation();
}

mi::Vector3f SionnaRadioDeviceBase::velocity() const
{
    return actor_->velocity();
}
