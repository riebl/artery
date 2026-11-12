#pragma once

#include <artery/sionna/bridge/Fwd.h>
#include <artery/sionna/bridge/bindings/Scene.h>
#include <mitsuba/core/fresolver.h>
#include <mitsuba/render/scene.h>

namespace artery::sionna
{

// Provides initial scene config.
class IStaticSceneProvider
{
public:
    // Return Mitsuba scene that will bootstrap the scene for Sionna.
    virtual mitsuba::ref<mitsuba::Resolve::Scene> getSceneConfig() = 0;

    virtual ~IStaticSceneProvider() = default;
};

// Simple RAII file resolver, used to set file resolver root for loading
// scene-local meshes. Most complex scenes use this to resolve mesh paths.
class ScopedFileResolver
{
public:
    // Initialize scoped resolver with a number of given paths. They will be appended, then removed
    // on destruction.
    static ScopedFileResolver withLocalPaths(std::initializer_list<mitsuba::fs::path> paths)
    {
        auto* previousResolver = mitsuba::file_resolver();
        mitsuba::ref<mitsuba::FileResolver> resolver = previousResolver ? new mitsuba::FileResolver(*previousResolver) : new mitsuba::FileResolver();

        for (const auto& path : paths) {
            resolver->append(path);
        }

        return ScopedFileResolver(resolver);
    }

    explicit ScopedFileResolver(mitsuba::FileResolver* resolver) : previous_(mitsuba::file_resolver()) { mitsuba::set_file_resolver(resolver); }

    ~ScopedFileResolver()
    {
        if (previous_ != nullptr) {
            mitsuba::set_file_resolver(previous_.get());
        }
    }

    ScopedFileResolver(const ScopedFileResolver&) = delete;
    ScopedFileResolver(ScopedFileResolver&& other) noexcept : previous_(std::move(other.previous_)) {}

    ScopedFileResolver& operator=(const ScopedFileResolver&) = delete;
    ScopedFileResolver& operator=(ScopedFileResolver&& other) noexcept
    {
        if (this != &other) {
            previous_ = std::move(other.previous_);
        }
        return *this;
    }

private:
    mitsuba::ref<mitsuba::FileResolver> previous_;
};

}  // namespace artery::sionna
