###############
# Main target #
###############

set(SIONNA_BRIDGE_COMPILATION_MODES Basic Inet)

# Triplets compiled for bridge, in form name, float type, spectrum type.
set(SIONNA_BRIDGE_TARGET_TRIPLETS
    "scalar|float|mitsuba::Color<float,3>"
    "llvm-detached|drjit::LLVMArray<float>|mitsuba::Color<drjit::LLVMArray<float>,3>"
    "llvm-ad|drjit::LLVMDiffArray<float>|mitsuba::Color<drjit::LLVMDiffArray<float>,3>"
    "llvm-ad-mono-polarized|drjit::LLVMDiffArray<float>|mitsuba::MuellerMatrix<mitsuba::Color<drjit::LLVMDiffArray<float>,1>>"
)

# Main target(s). There is a separate library for each compilation mode and
# resolve profile.
foreach(_mode IN ITEMS ${SIONNA_BRIDGE_COMPILATION_MODES})
  string(TOLOWER ${_mode} _mode_lowercase)

  foreach(_profile_triplet IN LISTS SIONNA_BRIDGE_TARGET_TRIPLETS)
    string(REPLACE "|" ";" _profile_parts ${_profile_triplet})
    list(GET _profile_parts 0 _profile_suffix)
    list(GET _profile_parts 1 _resolve_float)
    list(GET _profile_parts 2 _resolve_spectrum)

    set(_target sionna-bridge-${_mode_lowercase}-${_profile_suffix})
    add_library(${_target} SHARED)

    target_sources(${_target}
            PRIVATE
                bindings/AntennaArray.cc
                bindings/Camera.cc
                bindings/Constants.cc
                bindings/Material.cc
                bindings/PathSolver.cc
                bindings/Paths.cc
                bindings/RadioDevice.cc
                bindings/Scene.cc
                bindings/SceneObject.cc
            PUBLIC
                Fwd.h
                Helpers.h
                Compat.h
                Defaulted.h
                capabilities/Calling.h
                capabilities/Core.h
                capabilities/Defaulted.h
                bindings/AntennaArray.h
                bindings/Camera.h
                bindings/Constants.h
                bindings/Material.h
                bindings/PathSolver.h
                bindings/Paths.h
                bindings/RadioDevice.h
                bindings/Scene.h
                bindings/SceneObject.h
        )

    target_include_directories(${_target}
            PUBLIC
                ${CMAKE_SOURCE_DIR}/src
        )

    target_link_libraries(${_target}
            PUBLIC
                Mitsuba::nanobind-drjit
                Mitsuba::mitsuba
                drjit::drjit
                drjit::drjit-core
                drjit::drjit-extra
                tsl::robin_map
                Python::Python
                sionna-compilation-flags
                Backward::Backward
        )

    target_compile_options(${_target}
            PUBLIC
                $<$<CONFIG:Debug>:-DNDEBUG>
        )

    set_target_properties(${_target}
            PROPERTIES
                SIONNA_BRIDGE_COMPILATION_MODE ${_mode}
                INTERFACE_SIONNA_FLOAT_TYPE ${_resolve_float}
                INTERFACE_SIONNA_SPECTRUM_TYPE ${_resolve_spectrum}
        )

    # Propagate and enforce a single resolve profile across link interfaces.
    set_property(TARGET ${_target} APPEND PROPERTY INTERFACE_COMPATIBLE_INTERFACE_STRING SIONNA_FLOAT_TYPE)
    set_property(TARGET ${_target} APPEND PROPERTY INTERFACE_COMPATIBLE_INTERFACE_STRING SIONNA_SPECTRUM_TYPE)

    target_compile_definitions(${_target}
            PUBLIC
                SIONNA_BRIDGE_RESOLVE_FLOAT=${_resolve_float}
                SIONNA_BRIDGE_RESOLVE_SPECTRUM=${_resolve_spectrum}
        )

    # Post configuration for various compilation modes.
    if(_mode STREQUAL "Inet")
      target_link_libraries(${_target} PUBLIC INET)
    endif()
  endforeach()
endforeach()
