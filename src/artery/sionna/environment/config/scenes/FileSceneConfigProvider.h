#pragma once

#include <artery/sionna/environment/config/scenes/IStaticSceneProvider.h>
#include <mitsuba/core/filesystem.h>
#include <mitsuba/core/parser.h>
#include <omnetpp/csimplemodule.h>

namespace artery::sionna
{

// Bootstrap provider that loads a Mitsuba scene from a file path.
class FileSceneConfigProvider : public IStaticSceneProvider, public omnetpp::cSimpleModule
{
public:
    FileSceneConfigProvider() = default;

    // omnetpp::cSimpleModule implementation.
    void initialize() override;
    void initializeFromPathAndConfig(const mitsuba::fs::path& path, mitsuba::parser::ParserConfig cfg);

    // IStaticSceneProvider implementation.
    mitsuba::ref<mi::Scene> getSceneConfig() override;

private:
    mitsuba::fs::path path_;
    mitsuba::parser::ParserConfig config_{VariantName::name};
};

}  // namespace artery::sionna
