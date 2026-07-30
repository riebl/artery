#pragma once

#include <artery/sionna/bridge/capabilities/Core.h>

namespace artery::sionna::py
{

// Base identity for wrappers around classes exported from the main Sionna RT
// python package, e.g. Scene, Transmitter, Receiver, Paths.
class SionnaRtModule : public virtual IPythonModuleIdentityCapability
{
public:
    const char* moduleName() const override { return "sionna.rt"; }
};

// Base identity for wrappers around constants and enum-like values from
// sionna.rt.constants.
class SionnaRtConstantsModule : public virtual IPythonModuleIdentityCapability
{
public:
    const char* moduleName() const override { return "sionna.rt.constants"; }
};

// Base identity for wrappers that fetch built-in scene assets from
// sionna.rt.scenes.
class SionnaRtScenesModule : public virtual IPythonModuleIdentityCapability
{
public:
    const char* moduleName() const override { return "sionna.rt.scenes"; }
};

}  // namespace artery::sionna::py
