#include "SionnaBuiltinSceneConfigProvider.h"

#include <artery/sionna/environment/config/meshes/IMeshRegistry.h>
#include <mitsuba/core/filesystem.h>
#include <mitsuba/core/properties.h>

#include <sstream>
#include <unordered_map>

using namespace artery::sionna;

Define_Module(SionnaBuiltinSceneConfigProvider);

namespace
{

using path = mitsuba::fs::path;

const std::unordered_map<std::string, path> scenes = {{"munich", path("munich/munich.xml")}};

}  // namespace

void SionnaBuiltinSceneConfigProvider::initialize()
{
    std::string sceneName = par("scene").stdstringValue();
    if (auto scene = scenes.find(sceneName); scene == scenes.end()) {
        std::stringstream available;

        available << "[";
        for (const auto& [name, _] : scenes) {
            available << name << ", ";
        }
        available << "]";

        throw omnetpp::cRuntimeError(
            "failed to get scene from parameter: scene \"%s\" is not known. Available: %s", sceneName.c_str(), available.str().c_str());
    } else {
        initializeFromPathAndConfig(ScenesFileview::scenes() / scene->second, {VariantName::name});
    }
}
