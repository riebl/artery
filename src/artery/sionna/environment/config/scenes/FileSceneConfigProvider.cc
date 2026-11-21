#include "FileSceneConfigProvider.h"

#include <artery/sionna/bridge/Helpers.h>
#include <artery/sionna/environment/config/scenes/IStaticSceneProvider.h>
#include <mitsuba/core/fresolver.h>
#include <mitsuba/core/object.h>
#include <mitsuba/core/parser.h>
#include <omnetpp/cexception.h>

using namespace artery::sionna;

Define_Module(FileSceneConfigProvider);

namespace
{

mitsuba::ref<mi::Scene> instantiateScene(mitsuba::parser::ParserConfig& config, mitsuba::parser::ParserState& state)
{
    std::vector<mitsuba::ref<mitsuba::Object>> objects = mitsuba::parser::instantiate(config, state);
    if (objects.empty()) {
        throw wrapRuntimeError("sionna: parser did not instantiate any object");
    }

    if (auto* scene = dynamic_cast<mi::Scene*>(objects.front().get()); scene == nullptr) {
        throw wrapRuntimeError("sionna: root object is not a Mitsuba scene");
    } else {
        return mitsuba::ref<mi::Scene>(scene);
    }
}

}  // namespace

void FileSceneConfigProvider::initialize()
{
    auto path = mitsuba::fs::path(par("scenePath").stringValue());

    mitsuba::parser::ParserConfig cfg{VariantName::name};
    cfg.merge_meshes = par("mergeMeshes").boolValue();
    cfg.merge_equivalent = par("mergeEquivalent").boolValue();

    initializeFromPathAndConfig(path, std::move(cfg));
}

void FileSceneConfigProvider::initializeFromPathAndConfig(const mitsuba::fs::path& path, mitsuba::parser::ParserConfig cfg)
{
    path_ = path;
    config_ = std::move(cfg);
}

mitsuba::ref<mi::Scene> FileSceneConfigProvider::getSceneConfig()
{
    if (path_.empty()) {
        auto path = mitsuba::fs::path(par("scenePath").stringValue());

        mitsuba::parser::ParserConfig cfg{VariantName::name};
        cfg.merge_meshes = par("mergeMeshes").boolValue();
        cfg.merge_equivalent = par("mergeEquivalent").boolValue();

        initializeFromPathAndConfig(path, std::move(cfg));
    }

    if (path_.empty()) {
        throw omnetpp::cRuntimeError("failed to load a scene: configured path is empty");
    }

    auto resolver = ScopedFileResolver::withLocalPaths({path_.parent_path()});

    mitsuba::parser::ParserState state = mitsuba::parser::parse_file(config_, path_);
    mitsuba::parser::transform_all(config_, state);
    return instantiateScene(config_, state);
}
