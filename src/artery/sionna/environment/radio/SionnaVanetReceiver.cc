#include "SionnaVanetReceiver.h"

#include <omnetpp/cexception.h>

using namespace artery::sionna;

Define_Module(SionnaVanetReceiver);

void SionnaVanetReceiver::initialize(int stage)
{
    artery::VanetReceiver::initialize(stage);
    if (stage == inet::INITSTAGE_LAST) {
        initializeSionnaRadioDevice(this);
    }
}

void SionnaVanetReceiver::finish()
{
    if (device_.has_value()) {
        api()->dynamicConfiguration()->removeSceneItem(sceneID());
        device_.reset();
    }

    finishSionnaRadioDevice();
    artery::VanetReceiver::finish();
}

void SionnaVanetReceiver::bindIntoScene()
{
    auto* sceneConfig = api()->dynamicConfiguration();

    const auto position = this->position();
    const auto orientation = api()->coordinateTransform()->toSionnaScene(this->orientation());
    const auto velocity = api()->coordinateTransform()->toSionnaScene(this->velocity());

    device_ = py::Receiver(sceneID(), position, orientation, velocity);
    if (!sceneConfig->addReceiver(device_.value())) {
        throw omnetpp::cRuntimeError("Sionna scene already contains an item named %s", sceneID().c_str());
    }
}

void SionnaVanetReceiver::updatePhysics()
{
    auto& rx = device();

    const auto position = this->position();
    const auto orientation = this->orientation();
    const auto velocity = this->velocity();

    rx.setPosition(position);
    rx.setOrientation(api()->coordinateTransform()->toSionnaScene(orientation));
    rx.setVelocity(api()->coordinateTransform()->toSionnaScene(velocity));
}

py::Receiver& SionnaVanetReceiver::device()
{
    return device_.value();
}

const py::Receiver& SionnaVanetReceiver::device() const
{
    return device_.value();
}
