#include "TraciDynamicSceneConfigProvider.h"

#include <inet/common/ModuleAccess.h>
#include <omnetpp/ccomponent.h>
#include <omnetpp/cexception.h>
#include <traci/BasicNodeManager.h>

using namespace artery::sionna;

Define_Module(TraciDynamicSceneConfigProvider);

omnetpp::simsignal_t TraciDynamicSceneConfigProvider::sceneEdited = omnetpp::cComponent::registerSignal("sceneEdited");

void TraciDynamicSceneConfigProvider::initialize()
{
    api_ = ISionnaAPI::get(this);
    traciNodeManager_ = inet::getModuleFromPar<traci::BasicNodeManager>(par("traciNodeManagerModule"), this);
    traciNodeManager_->subscribe(traci::BasicNodeManager::updateNodeSignal, this);
}

void TraciDynamicSceneConfigProvider::finish()
{
    traciNodeManager_->unsubscribe(traci::BasicNodeManager::updateNodeSignal, this);
}

void TraciDynamicSceneConfigProvider::receiveSignal(
    omnetpp::cComponent* /* source */, omnetpp::simsignal_t signal, unsigned long /* value */, omnetpp::cObject* /* details */)
{
    // This signals end-of-step, so scene is flushed here.
    if (signal == traci::BasicNodeManager::updateNodeSignal) {
        edit();
    } else {
        throw omnetpp::cRuntimeError("could not dispatch signal: unknown signal received");
    }
}

void TraciDynamicSceneConfigProvider::edit()
{
    api_->dynamicConfiguration()->edit();
    emit(sceneEdited, 1UL);
}
