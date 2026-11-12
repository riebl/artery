#pragma once

// clang-format off
#if !defined(SIONNA_BRIDGE_COMPILATION_MODE_BASIC) && !defined(SIONNA_BRIDGE_COMPILATION_MODE_INET)
    #error "Compilation mode is missing. Cannot continue"
#endif

#if !defined(SIONNA_BRIDGE_RESOLVE_FLOAT)
    #if defined(__clang__) || defined(__GNUC__)
        #warning "Cannot detect mitsuba Float parameter, bridge module falls back to scalar type"
    #elif defined(_MSC_VER)
        #pragma message("Cannot detect mitsuba Float parameter, bridge module falls back to scalar type")
    #endif
#endif

#if !defined(SIONNA_BRIDGE_RESOLVE_SPECTRUM)
    #if defined(__clang__) || defined(__GNUC__)
        #warning "Cannot detect mitsuba Spectrum parameter, bridge module falls back to scalar type"
    #elif defined(_MSC_VER)
        #pragma message("Cannot detect mitsuba Spectrum parameter, bridge module falls back to scalar type")
    #endif
#endif
// clang-format on

#include <artery/sionna/bridge/Casters.h>
#include <artery/sionna/bridge/Compat.h>
#include <artery/sionna/bridge/Defaulted.h>
#include <artery/sionna/bridge/Fwd.h>
#include <artery/sionna/bridge/Helpers.h>
#include <artery/sionna/bridge/bindings/AntennaArray.h>
#include <artery/sionna/bridge/bindings/Camera.h>
#include <artery/sionna/bridge/bindings/Constants.h>
#include <artery/sionna/bridge/bindings/Material.h>
#include <artery/sionna/bridge/bindings/Modules.h>
#include <artery/sionna/bridge/bindings/PathSolver.h>
#include <artery/sionna/bridge/bindings/Paths.h>
#include <artery/sionna/bridge/bindings/RadioDevice.h>
#include <artery/sionna/bridge/bindings/Scene.h>
#include <artery/sionna/bridge/bindings/SceneObject.h>
#include <artery/sionna/bridge/capabilities/Calling.h>
#include <artery/sionna/bridge/capabilities/Core.h>
#include <artery/sionna/bridge/capabilities/Defaulted.h>
