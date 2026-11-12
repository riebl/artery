#include "SionnaVanetTransmitter.h"

#include <omnetpp/cexception.h>

using namespace artery::sionna;

Define_Module(SionnaVanetTransmitter);

void SionnaVanetTransmitter::initialize(int stage)
{
    inet::physicallayer::Ieee80211ScalarTransmitter::initialize(stage);
    if (stage == inet::INITSTAGE_LAST) {
        initializeSionnaRadioDevice(this);
    }
}

void SionnaVanetTransmitter::finish()
{
    if (device_.has_value()) {
        api()->dynamicConfiguration()->removeSceneItem(sceneID());
        device_.reset();
    }

    finishSionnaRadioDevice();
    inet::physicallayer::Ieee80211ScalarTransmitter::finish();
}

void SionnaVanetTransmitter::bindIntoScene()
{
    auto* sceneConfig = api()->dynamicConfiguration();

    const auto position = this->position();
    const auto orientation = api()->coordinateTransform()->toSionnaScene(this->orientation());
    const auto velocity = api()->coordinateTransform()->toSionnaScene(this->velocity());

    device_ = py::Transmitter(sceneID(), position, orientation, velocity);
    if (!sceneConfig->addTransmitter(device_.value())) {
        throw omnetpp::cRuntimeError("Sionna scene already contains an item named %s", sceneID().c_str());
    }
}

void SionnaVanetTransmitter::updatePhysics()
{
    auto& tx = device();

    const auto position = this->position();
    const auto orientation = this->orientation();
    const auto velocity = this->velocity();

    tx.setPosition(position);
    tx.setOrientation(api()->coordinateTransform()->toSionnaScene(orientation));
    tx.setVelocity(api()->coordinateTransform()->toSionnaScene(velocity));
}

py::Transmitter& SionnaVanetTransmitter::device()
{
    return device_.value();
}

const py::Transmitter& SionnaVanetTransmitter::device() const
{
    return device_.value();
}
