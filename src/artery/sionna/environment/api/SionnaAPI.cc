#include "SionnaAPI.h"

#include <artery/sionna/environment/PhysicalEnvironment.h>
#include <mitsuba/render/scene.h>
#include <omnetpp/cexception.h>

using namespace artery::sionna;

ISionnaAPI* ISionnaAPI::get(const omnetpp::cModule* module)
{
    // TODO: make Sionna API resolution const-correct instead of casting away constness.
    auto* mutableModule = const_cast<omnetpp::cModule*>(module);
    if (auto* api = dynamic_cast<ISionnaAPI*>(mutableModule); api != nullptr) {
        return api;
    }

    auto* root = mutableModule != nullptr ? mutableModule->getSimulation()->getSystemModule() : nullptr;
    auto* physicalEnvironment = root != nullptr ? root->getSubmodule("physicalEnvironment") : nullptr;
    if (auto* api = dynamic_cast<ISionnaAPI*>(physicalEnvironment); api != nullptr) {
        return api;
    }

    throw omnetpp::cRuntimeError("could not resolve Sionna API from module %s", module != nullptr ? module->getFullPath().c_str() : "<null>");
}
