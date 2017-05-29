project(Artery)
cmake_minimum_required(VERSION 3.0)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_CXX_STANDARD 11)

find_package(OmnetPP 5.1 REQUIRED)
include(cmake/ImportOppTarget.cmake)
include(cmake/GetNedFolders.cmake)
include(cmake/AddOppRun.cmake)

find_package(Vanetza CONFIG PATHS extern/vanetza/build REQUIRED NO_CMAKE_PACKAGE_REGISTRY)

find_path(Veins_DIR NAMES src/veins/package.ned PATHS extern/veins DOC "Veins root directory")
import_opp_target(veins ${Veins_DIR}/src/Makefile)

find_path(INET_DIR NAMES src/inet/package.ned PATHS extern/inet DOC "INET root directory")
import_opp_target(INET ${INET_DIR}/src/Makefile)

find_program(MAKE_COMMAND NAMES make gmake HINTS ${CMAKE_MAKE_COMMAND} DOC "Makefile processor")
set(MAKE_ARGUMENTS "" CACHE STRING "Additional arguments passed to Makefile processor")
string(REPLACE " " ";" MAKE_ARGUMENTS "${MAKE_ARGUMENTS}")

if("${Veins_DIR}" STREQUAL "${PROJECT_SOURCE_DIR}/extern/veins")
    set(_veins_mode "release")
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(_veins_mode "debug")
    endif()

    set(_make_command ${MAKE_COMMAND})
    if(CMAKE_GENERATOR MATCHES "Makefiles")
        set(_make_command \$\(MAKE\))
    endif()

    add_custom_target(build_veins
        COMMAND ${_make_command} ${MAKE_ARGUMENTS} veins MODE=${_veins_mode}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMENT "Building Veins (external dependency)"
        VERBATIM)
    add_dependencies(veins build_veins)
endif()

if("${INET_DIR}" STREQUAL "${PROJECT_SOURCE_DIR}/extern/inet")
    set(_inet_mode "release")
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(_inet_mode "debug")
    endif()

    set(_make_command ${MAKE_COMMAND})
    if(CMAKE_GENERATOR MATCHES "Makefiles")
        set(_make_command \$\(MAKE\))
    endif()

    add_custom_target(build_inet
        COMMAND ${_make_command} ${MAKE_ARGUMENTS} inet MODE=${_inet_mode}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMENT "Building INET (external dependency)"
        VERBATIM)
    add_dependencies(INET build_inet)
endif()

find_package(Boost COMPONENTS python QUIET)
set(_storyboard_default OFF)
if(Boost_PYTHON_FOUND)
    set(_storyboard_default ON)
endif()
option(WITH_STORYBOARD "Build Artery with storyboard feature" ${_storyboard_default})
option(WITH_TRANSFUSION "Build Artery with transfusion feature" OFF)

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
