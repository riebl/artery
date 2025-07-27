cmake_minimum_required(VERSION 3.16)
cmake_policy(VERSION 3.16...3.31)

project(Artery)

option(WITH_ENVMOD "Build Artery with environment model feature" ON)
option(WITH_STORYBOARD "Build Artery with storyboard feature" ON)
option(WITH_TRANSFUSION "Build Artery with transfusion feature" OFF)
option(WITH_TESTBED "Build Artery with testbed feature" OFF)
option(WITH_PYTHON_BINDINGS "Build Artery with python bindings" ON)
option(WITH_OTS "Build Artery with support for OpenTrafficSim" OFF)
option(WITH_SIMULTE "Build Artery with SimuLTE integration" OFF)
option(VSCODE_LAUNCH_INTEGRATION "Generate VS Code configuration for debugging Artery (requires debug build)" OFF)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

find_package(OmnetPP 5.5.1 MODULE REQUIRED)

include(AddOppRun)
include(AddOppTarget)
include(AddVSCode)
include(CheckGitSubmodule)
include(GenerateOppMessage)
include(GNUInstallDirs)

find_package(Boost 1.59 REQUIRED COMPONENTS date_time system CONFIG)

# use "RelWithDebInfo" as default build type
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "CMake build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# running simulations with opp_run requires shared libraries
set(BUILD_SHARED_LIBS ON CACHE INTERNAL "Cached for propagation to sub-projects with older CMake versions")

add_subdirectory(extern)

if(WITH_OTS)
    add_subdirectory(src/ots)
endif()

add_subdirectory(src/traci)
add_subdirectory(src/artery)

# scenarios directory is part of repository but omitted for Docker build context
if(EXISTS ${PROJECT_SOURCE_DIR}/scenarios)
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
