cmake_minimum_required(VERSION 3.20)

cmake_policy(VERSION 3.20...3.31)

project(Artery LANGUAGES CXX C)

enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

##################
# Artery Options #
##################

# These are essential componets that are better to be turned on.
option(WITH_INET "Build Artery with INET framework integration" ON)
option(WITH_VEINS "Build Artery with Veins framework integration" ON)

# Various extensions, adjust them as needed.
option(WITH_SIMULTE "Build Artery with SimuLTE integration" OFF)
option(WITH_OTS "Build Artery with support for OpenTrafficSim" OFF)
option(WITH_TESTBED "Build Artery with testbed feature" OFF)
option(WITH_ENVMOD "Build Artery with environment model feature" ON)
option(WITH_STORYBOARD "Build Artery with storyboard feature" ON)
option(WITH_TRANSFUSION "Build Artery with transfusion feature" OFF)

# Miscellaneous stuff, does not affect functionality directly.
option(WITH_SCENARIOS "Build Artery with scenarios" ON)
option(VSCODE_LAUNCH_INTEGRATION "Generate VS Code configuration for debugging Artery (requires debug build)" OFF)

##############################
# Artery build configuration #
##############################

# use "RelWithDebInfo" as default build type
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "CMake build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# running simulations with opp_run requires shared libraries
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(BUILD_SHARED_LIBS ON CACHE INTERNAL "Cached for propagation to sub-projects with older CMake versions")

###############################
# Required packages & scripts #
###############################

find_package(OmnetPP 5.5.1 MODULE REQUIRED)
find_package(Boost 1.59 REQUIRED COMPONENTS date_time system CONFIG)

include(AddOppRun)
include(AddOppTarget)
include(AddVSCode)
include(CheckGitSubmodule)
include(ArteryCodegen)
include(AddArterySubdirectory)
include(GNUInstallDirs)

# Packages below are needed by various
# optional components. They should be used
# with add_artery_subdirectory() to trigger
# checks on them only if variables (WITH*)
# are set.

find_package(Protobuf QUIET)
find_package(PkgConfig MODULE QUIET)
find_package(SEA_V2X CONFIG QUIET)

if(PkgConfig_FOUND)
    pkg_check_modules(ZEROMQ QUIET IMPORTED_TARGET libzmq)
endif()

if(SEA_V2X_FOUND)
    mark_as_advanced(SEA_V2X_DIR)
endif()

if(NOT Protobuf_PROTOC_EXECUTABLE)
    find_program(Protobuf_PROTOC_EXECUTABLE protoc REQUIRED)
endif()

add_subdirectory(extern)

##########################
# Primary Artery targets #
##########################

add_library(artery INTERFACE)

add_library(core SHARED)
add_library(Artery::Core ALIAS core)

macro(add_artery_feature name)
    add_library(${name} SHARED ${ARGN})
    add_library(Artery::${name} ALIAS ${name})
    get_target_property(ned_folder ${name} SOURCE_DIR)
    set_target_properties(${name} PROPERTIES
        OUTPUT_NAME artery_${name}
        OMNETPP_LIBRARY ON
        NED_FOLDERS ${ned_folder})
    target_link_libraries(${name} PRIVATE core)
    target_link_libraries(artery INTERFACE Artery::${name})
    install(TARGETS ${name} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
endmacro()

##########
# Artery #
##########

add_subdirectory(src/traci)
add_subdirectory(src/artery)

add_artery_subdirectory(src/ots REQUIRES INET PkgConfig::ZEROMQ SWITCH WITH_OTS)

# scenarios directory is part of repository but omitted for Docker build context
if(WITH_SCENARIOS)
    add_subdirectory(scenarios)
endif()

get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

if(is_multi_config)
    foreach(config IN LISTS CMAKE_CONFIGURATION_TYPES)
        string(TOLOWER "${config}" config_lower)
        generate_run_config(TARGET artery CONFIG ${config} FILE "${PROJECT_BINARY_DIR}/run-artery.${config}.ini")
        generate_run_config(TARGET artery CONFIG ${config} FILE "${PROJECT_BINARY_DIR}/run-artery.${config}.ini.install" INSTALL)
    endforeach()
else()
    generate_run_config(TARGET artery CONFIG ${CMAKE_BUILD_TYPE} FILE "${PROJECT_BINARY_DIR}/run-artery.ini")
    generate_run_config(TARGET artery CONFIG ${CMAKE_BUILD_TYPE} FILE "${PROJECT_BINARY_DIR}/run-artery.ini.install" INSTALL)
endif()

if(VSCODE_LAUNCH_INTEGRATION)
    if(is_multi_config AND "Debug" IN_LIST CMAKE_CONFIGURATION_TYPES)
        message(STATUS "VS Code integration is enabled, but Debug is not included in build types")
    elseif(NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        message(STATUS "VS Code integration is enabled, but build type is not Debug")
    else()
        generate_vscode(TARGET artery FILE ${PROJECT_SOURCE_DIR}/.vscode/launch.json)
    endif()
endif()
