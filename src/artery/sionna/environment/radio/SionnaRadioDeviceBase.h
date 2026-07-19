#pragma once

#include <artery/sionna/bridge/SionnaBridge.h>
#include <artery/sionna/environment/api/SionnaAPI.h>
#include <artery/sionna/environment/config/dynamic/DynamicSceneConfigListener.h>
#include <artery/sionna/environment/config/dynamic/ISionnaActor.h>
#include <inet/common/INETDefs.h>
#include <inet/mobility/contract/IMobility.h>
#include <inet/physicallayer/contract/packetlevel/IRadio.h>
#include <omnetpp/cmodule.h>

namespace artery::sionna
{

// Shared Sionna scene tracking for INET radio transmitter/receiver modules.
class SionnaRadioDeviceBase : public DynamicSceneConfigListener
{
public:
    // Stable Sionna scene ID derived from the owning OMNeT++ module path.
    std::string sceneID() const;
    // Parent INET radio that owns this transmitter/receiver module.
    const inet::physicallayer::IRadio* radio() const;

protected:
    // Bind tracking to the concrete transmitter/receiver module.
    void initializeSionnaRadioDevice(omnetpp::cModule* owner);
    // Release dynamic-scene subscriptions and actor references.
    void finishSionnaRadioDevice();

    inet::IMobility* mobility() const;
    ISionnaAPI* api() const;
    // SUMO-backed scene actor owning the vehicle/person object.
    ISionnaActor* actor() const;

    // Current Sionna scene-space radio device state.
    mi::Point3f position() const;
    mi::Point3f orientation() const;
    mi::Vector3f velocity() const;

    // Add concrete transmitter/receiver device to Sionna.
    virtual void bindIntoScene() = 0;
    // Push latest actor-derived transform into the Sionna device.
    virtual void updatePhysics() = 0;

    void onDynamicSceneEdited() override;

private:
    omnetpp::cModule* owner_ = nullptr;
    inet::IMobility* mobility_ = nullptr;
    ISionnaActor* actor_ = nullptr;
};

}  // namespace artery::sionna
