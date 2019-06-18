project(Artery)
cmake_minimum_required(VERSION 3.1)
if(POLICY CMP0076)
    cmake_policy(SET CMP0076 NEW)
endif()
if(POLICY CMP0078)
    cmake_policy(SET CMP0079 OLD)
endif()
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_CXX_STANDARD 11)
enable_testing()

find_package(OmnetPP 5.4 REQUIRED)
include(cmake/ImportOppTarget.cmake)
include(cmake/GenerateOppMessage.cmake)
include(cmake/GetNedFolders.cmake)
include(cmake/AddOppBuildTarget.cmake)
include(cmake/AddOppRun.cmake)

find_package(Vanetza CONFIG PATHS extern/vanetza/build REQUIRED NO_CMAKE_PACKAGE_REGISTRY)

find_path(Veins_DIR NAMES src/veins/package.ned PATHS extern/veins DOC "Veins root directory")
import_opp_target(veins ${Veins_DIR}/src/Makefile)

find_path(INET_DIR NAMES src/inet/package.ned PATHS extern/inet DOC "INET root directory")
import_opp_target(INET ${INET_DIR}/src/Makefile)

option(WITH_SIMULTE "Build Artery with SimuLTE integration" OFF)
if(WITH_SIMULTE)
    find_path(SimuLTE_DIR NAMES src/package.ned PATHS extern/simulte DOC "SimuLTE root directory")
    import_opp_target(lte ${SimuLTE_DIR}/src/Makefile)
else()
    message(STATUS "SimuLTE integration disabled")
endif()

find_program(MAKE_COMMAND NAMES make gmake HINTS ${CMAKE_MAKE_COMMAND} DOC "Makefile processor")
set(MAKE_ARGUMENTS "" CACHE STRING "Additional arguments passed to Makefile processor")
string(REPLACE " " ";" MAKE_ARGUMENTS "${MAKE_ARGUMENTS}")

if("${Vanetza_DIR}" STREQUAL "${PROJECT_SOURCE_DIR}/extern/vanetza/build")
    set(_make_command ${MAKE_COMMAND})
    if(CMAKE_GENERATOR MATCHES "Makefiles")
        set(_make_command \$\(MAKE\))
    endif()
    add_custom_target(build_vanetza
        COMMAND ${_make_command} ${MAKE_ARGUMENTS} vanetza
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMENT "Building Vanetza (external dependency)"
        VERBATIM)
    add_dependencies(Vanetza::vanetza build_vanetza)
endif()

if("${Veins_DIR}" STREQUAL "${PROJECT_SOURCE_DIR}/extern/veins")
    add_opp_build_target(Veins)
    add_dependencies(veins build_veins)
endif()

if("${INET_DIR}" STREQUAL "${PROJECT_SOURCE_DIR}/extern/inet")
    add_opp_build_target(INET)
    add_dependencies(INET build_inet)
endif()

if("${SimuLTE_DIR}" STREQUAL "${PROJECT_SOURCE_DIR}/extern/simulte" AND WITH_SIMULTE)
    add_opp_build_target(SimuLTE)
    add_dependencies(lte build_simulte)
endif()

option(WITH_ENVMOD "Build Artery with environment model feature" ON)
option(WITH_STORYBOARD "Build Artery with storyboard feature" ON)
option(WITH_TRANSFUSION "Build Artery with transfusion feature" OFF)
option(WITH_TESTBED "Build Artery with testbed feature" OFF)

add_subdirectory(src/artery)
add_subdirectory(src/traci)
add_subdirectory(scenarios)

# sumo-launchd should be run from root directory for correct file look-up
set(SUMO_LAUNCHD_ARGUMENTS "--daemon --kill" CACHE STRING "Additional arguments passed to sumo-launchd")
string(REPLACE " " ";" _sumo_launchd_args "${SUMO_LAUNCHD_ARGUMENTS}")
add_custom_target(launch_sumo
    COMMAND ${Veins_DIR}/sumo-launchd.py ${_sumo_launchd_args}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMENT "Launching SUMO"
    VERBATIM)
