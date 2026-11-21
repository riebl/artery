#pragma once

#include <artery/sionna/environment/api/SionnaAPI.h>
#include <omnetpp/clistener.h>
#include <omnetpp/csimplemodule.h>
#include <traci/BasicNodeManager.h>

namespace artery::sionna
{

// Dynamic scene provider driven by TraCI node lifecycle signals.
class TraciDynamicSceneConfigProvider : public omnetpp::cSimpleModule, public omnetpp::cListener
{
public:
    static omnetpp::simsignal_t sceneEdited;

    TraciDynamicSceneConfigProvider() = default;

    // omnetpp::cSimpleModule implementation.
    void initialize() override;
    void finish() override;

    // omnetpp::cListener implementation.
    void receiveSignal(omnetpp::cComponent* /* source */, omnetpp::simsignal_t signal, unsigned long value, omnetpp::cObject* /* details */) override;

private:
    void edit();

private:
    ISionnaAPI* api_ = nullptr;
    traci::BasicNodeManager* traciNodeManager_ = nullptr;
};

}  // namespace artery::sionna
