#include "DynamicSceneConfigListener.h"

#include <artery/sionna/environment/api/SionnaAPI.h>
#include <artery/sionna/environment/config/dynamic/TraciDynamicSceneConfigProvider.h>
#include <omnetpp/cexception.h>
#include <omnetpp/cmodule.h>

using namespace artery::sionna;

void DynamicSceneConfigListener::subscribeToDynamicSceneUpdates(omnetpp::cModule* context)
{
    auto* apiModule = dynamic_cast<omnetpp::cModule*>(ISionnaAPI::get(context));
    auto* providerModule = apiModule != nullptr ? apiModule->getSubmodule("dynamicSceneConfigProvider") : nullptr;
    dynamicSceneConfigProvider_ = dynamic_cast<TraciDynamicSceneConfigProvider*>(providerModule);
    if (dynamicSceneConfigProvider_ == nullptr) {
        throw omnetpp::cRuntimeError("could not resolve Sionna dynamic scene config provider from module %s", context->getFullPath().c_str());
    }

    dynamicSceneConfigProvider_->subscribe(TraciDynamicSceneConfigProvider::sceneEdited, this);
}

void DynamicSceneConfigListener::unsubscribeFromDynamicSceneUpdates()
{
    if (dynamicSceneConfigProvider_ != nullptr) {
        dynamicSceneConfigProvider_->unsubscribe(TraciDynamicSceneConfigProvider::sceneEdited, this);
        dynamicSceneConfigProvider_ = nullptr;
    }
}

void DynamicSceneConfigListener::receiveSignal(
    omnetpp::cComponent* /* source */, omnetpp::simsignal_t signal, unsigned long /* value */, omnetpp::cObject* /* details */)
{
    if (signal == TraciDynamicSceneConfigProvider::sceneEdited) {
        onDynamicSceneEdited();
    }
}
