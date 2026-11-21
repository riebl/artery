#pragma once

#include <omnetpp/ccomponent.h>
#include <omnetpp/clistener.h>
#include <omnetpp/cmodule.h>

namespace artery::sionna
{

class TraciDynamicSceneConfigProvider;

// Listener for dynamic scene updates.
class DynamicSceneConfigListener : public omnetpp::cListener
{
public:
    // Subscribe to updates from Dynamic Scene Configurator.
    void subscribeToDynamicSceneUpdates(omnetpp::cModule* publisher);
    // Unsubscribe from Dynamic Scene Configurator updates.
    void unsubscribeFromDynamicSceneUpdates();

    // omnetpp::cListener implementation.
    void receiveSignal(omnetpp::cComponent* source, omnetpp::simsignal_t signal, unsigned long value, omnetpp::cObject* details) override;

protected:
    // This callback is invoked when mitsuba scene is rebuilt.
    virtual void onDynamicSceneEdited() = 0;

private:
    TraciDynamicSceneConfigProvider* dynamicSceneConfigProvider_ = nullptr;
};

}  // namespace artery::sionna
