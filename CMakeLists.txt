project(Artery)
cmake_minimum_required(VERSION 3.0)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package(OmnetPP REQUIRED)

find_package(PythonInterp 3 REQUIRED)
macro(import_opp_target _target _opp_makefile)
    # _cmake_target: generated CMake file with import target [optional argument]
    if(${ARGC} GREATER 2)
        set(_cmake_target "${ARGV2}")
    else()
        set(_cmake_target "${PROJECT_BINARY_DIR}/${_target}-targets.cmake")
    endif()

    # opp_makemake generated Makefile is required for proceeding
    if(NOT EXISTS ${_opp_makefile})
        message(FATAL_ERROR "Cannot import ${_target} because there is no opp_makemake file at ${_opp_makefile}")
    endif()

    # generate target file (prior to build system generation)
    execute_process(
        COMMAND ${PYTHON_EXECUTABLE} opp_cmake.py ${_opp_makefile} ${_cmake_target}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

    # include import targets if generated successfully
    if(EXISTS ${_cmake_target})
        include(${_cmake_target})
    else()
        message(FATAL_ERROR "There is no ${_cmake_target} for OMNeT++ import of ${_target}")
    endif()

    # sanity check: included target file has to provide the expected target
    if(NOT TARGET ${_target})
        message(FATAL_ERROR "Import of target ${_target} from ${_opp_makefile} failed")
    endif()
endmacro()

macro(get_ned_folders _target _output)
    get_target_property(_target_ned_folders ${_target} NED_FOLDERS)
    if(_target_ned_folders)
        set(${_output} ${_target_ned_folders})
    endif()

    get_target_property(_target_dependencies ${_target} INTERFACE_LINK_LIBRARIES)
    if(_target_dependencies)
        foreach(_target_dependency IN LISTS _target_dependencies)
            if(TARGET ${_target_dependency})
                get_ned_folders(${_target_dependency} _target_dependency_ned_folders)
                list(APPEND ${_output} ${_target_dependency_ned_folders})
            endif()
        endforeach()
    endif()

    if(${_output})
        list(REMOVE_DUPLICATES ${_output})
    endif()
endmacro()

find_program(OMNETPP_RUN NAME opp_run PATHS ${OMNETPP_ROOT}/bin DOC "OMNeT++ opp_run executable")
macro(add_opp_run _target _config)
    get_ned_folders(${_target} _list_ned_folders)
    list(APPEND _list_ned_folders ${ARGN})
    foreach(_ned_folder IN LISTS _list_ned_folders)
        set(_ned_folders "${_ned_folders}:${_ned_folder}")
    endforeach()
    if(_ned_folders)
        string(SUBSTRING ${_ned_folders} 1 -1 _ned_folders)
    endif()
    add_custom_target(run_${_target}
        COMMAND ${OMNETPP_RUN} -n ${_ned_folders} -l $<TARGET_FILE:${_target}> ${_config}
        SOURCES ${_config}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        VERBATIM)
endmacro()

find_package(Vanetza REQUIRED)

find_path(Veins_DIR NAMES src/veins/package.ned DOC "Veins root directory")
import_opp_target(veins ${Veins_DIR}/src/Makefile)

add_subdirectory(src/artery)
add_subdirectory(example)
