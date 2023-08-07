cmake_minimum_required(VERSION 3.4)
project(Artery)

if(POLICY CMP0076)
    cmake_policy(SET CMP0076 NEW)
endif()
if(POLICY CMP0079)
    if (CMAKE_VERSION VERSION_LESS 3.17)
        cmake_policy(SET CMP0079 OLD)
    else()
        cmake_policy(SET CMP0079 NEW)
    endif()
endif()

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
enable_testing()

find_package(OmnetPP 5.5.1 MODULE REQUIRED)
include(AddOppRun)
include(AddOppTarget)
include(AddVSCode)
include(CheckGitSubmodule)
include(GenerateOppMessage)
include(GNUInstallDirs)

find_package(Boost 1.59 COMPONENTS date_time system REQUIRED)
if (Boost_VERSION_STRING VERSION_GREATER_EQUAL "1.75")
    # Boost.Geometry requires C++14 starting with Boost 1.75
    set(CMAKE_CXX_STANDARD 14)
endif()

# running simulations with opp_run requires shared libraries
set(BUILD_SHARED_LIBS ON CACHE INTERNAL "Cached for propagation to sub-projects with older CMake versions")

check_git_submodule(PATH extern/vanetza REQUIRED_FILES CMakeLists.txt)
set(VANETZA_INSTALL ON)
add_subdirectory(${PROJECT_SOURCE_DIR}/extern/vanetza)
mark_as_advanced(BUILD_BENCHMARK BUILD_CERTIFY BUILD_SOCKTAP BUILD_TESTS BUILD_USING_CONAN)
mark_as_advanced(VANETZA_ASN1_WITH_ASN1C VANETZA_EXPORT_PACKAGE VANETZA_NET_WITH_POSIX)

check_git_submodule(PATH extern/veins REQUIRED_FILES src/veins/package.ned)
find_path(Veins_DIR NAMES src/veins/package.ned PATHS extern/veins DOC "Veins root directory")
mark_as_advanced(Veins_DIR)
add_opp_target(TARGET veins ROOT_DIR ${Veins_DIR})


check_git_submodule(PATH extern/inet REQUIRED_FILES src/inet/package.ned)
find_path(INET_DIR NAMES src/inet/package.ned PATHS extern/inet DOC "INET root directory")
mark_as_advanced(INET_DIR)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${INET_DIR}/.oppfeaturestate ${INET_DIR}/.nedexclusions)

# collect INET's opp_makemake options
execute_process(COMMAND ./inet_featuretool options -l -f
    WORKING_DIRECTORY ${INET_DIR}
    OUTPUT_VARIABLE INET_OPP_MAKEMAKE)
separate_arguments(INET_OPP_MAKEMAKE UNIX_COMMAND ${INET_OPP_MAKEMAKE})

# generate INET's features and opp_defines header
set(INET_GEN_DIR ${PROJECT_BINARY_DIR}/INET_gen)
file(MAKE_DIRECTORY ${INET_GEN_DIR}/inet)
add_custom_command(OUTPUT ${INET_DIR}/.oppfeaturestate
    COMMAND ./inet_featuretool repair
    DEPENDS ${INET_DIR}/.oppfeatures ${INET_DIR}/inet_featuretool
    WORKING_DIRECTORY ${INET_DIR})
add_custom_command(OUTPUT ${INET_GEN_DIR}/inet/features.h
    COMMAND ./inet_featuretool defines >${INET_GEN_DIR}/inet/features.h
    DEPENDS ${INET_DIR}/.oppfeaturestate
    WORKING_DIRECTORY ${INET_DIR}
    COMMENT "Generate INET features.h")
add_custom_command(OUTPUT ${INET_GEN_DIR}/inet/opp_defines.h
    COMMAND ${CMAKE_COMMAND} -E touch ${INET_GEN_DIR}/inet/opp_defines.h
    COMMENT "Generate INET opp_defines.h")

add_opp_target(TARGET INET ROOT_DIR ${INET_DIR}
    DEPENDS ${INET_GEN_DIR}/inet/features.h ${INET_GEN_DIR}/inet/opp_defines.h
    OPP_MAKEMAKE ${INET_OPP_MAKEMAKE})


check_git_submodule(PATH extern/simulte REQUIRED_FILES src/package.ned)
find_path(SimuLTE_DIR NAMES src/package.ned PATHS extern/simulte DOC "SimuLTE root directory")
mark_as_advanced(SimuLTE_DIR)
option(WITH_SIMULTE "Build Artery with SimuLTE integration" OFF)
if(WITH_SIMULTE)
    add_opp_target(TARGET lte ROOT_DIR ${SimuLTE_DIR})
    target_link_libraries(lte PUBLIC INET)
else()
    message(STATUS "SimuLTE integration disabled")
endif()


option(WITH_ENVMOD "Build Artery with environment model feature" ON)
option(WITH_STORYBOARD "Build Artery with storyboard feature" ON)
option(WITH_TRANSFUSION "Build Artery with transfusion feature" OFF)
option(WITH_TESTBED "Build Artery with testbed feature" OFF)

option(WITH_OTS "Build Artery with support for OpenTrafficSim" OFF)
if(WITH_OTS)
    add_subdirectory(src/ots)
endif()
add_subdirectory(src/traci)

add_subdirectory(src/artery)

option(VSCODE_LAUNCH_INTEGRATION "Generate VS Code configuration for debugging Artery (requires debug build)" OFF)

# scenarios directory is part of repository but omitted for Docker build context
if(EXISTS ${PROJECT_SOURCE_DIR}/scenarios)
    add_subdirectory(scenarios)
endif()
