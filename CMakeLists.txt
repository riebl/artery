project(Artery)
cmake_minimum_required(VERSION 3.0)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_CXX_STANDARD 11)

find_package(OmnetPP 4.6 REQUIRED)

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
        ERROR_VARIABLE _opp_cmake_error
        RESULT_VARIABLE _opp_cmake_result
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
    if(_opp_cmake_result)
        message(STATUS "opp_cmake generated following error log")
        message(SEND_ERROR ${_opp_cmake_error})
        message(FATAL_ERROR "import_opp_target for ${_target} aborted due to above opp_cmake error")
    endif()

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
    get_target_property(_target_type ${_target} TYPE)
    if(NOT ${_target_type} STREQUAL "INTERFACE_LIBRARY")
        get_target_property(_target_ned_folders ${_target} NED_FOLDERS)
        if(_target_ned_folders)
            list(APPEND ${_output} ${_target_ned_folders})
        endif()
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

find_program(OMNETPP_RUN NAMES opp_run opp_run_release PATHS ${OMNETPP_ROOT}/bin DOC "OMNeT++ opp_run executable")
macro(add_opp_run _name _config _target)
    get_ned_folders(${_target} _list_ned_folders)
    list(APPEND _list_ned_folders ${ARGN})
    foreach(_ned_folder IN LISTS _list_ned_folders)
        set(_ned_folders "${_ned_folders}:${_ned_folder}")
    endforeach()
    if(_ned_folders)
        string(SUBSTRING ${_ned_folders} 1 -1 _ned_folders)
    endif()

    get_target_property(_target_type ${_target} TYPE)
    if(${_target_type} STREQUAL "EXECUTABLE")
        set(_exec $<TARGET_FILE:${_target}> -n ${_ned_folders})
    else()
        set(_exec ${OMNETPP_RUN} -n ${_ned_folders} -l $<TARGET_FILE:${_target}>)
    endif()

    add_custom_target(run_${_name}
        COMMAND ${_exec} ${_config}
        SOURCES ${_config}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        VERBATIM)

    find_program(GDB_COMMAND gdb DOC "GNU debugger")
    if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND GDB_COMMAND)
        add_custom_target(debug_${_name}
            COMMAND ${GDB_COMMAND} --args ${_exec} ${_config}
            SOURCES ${_config}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            VERBATIM)
    endif()

    find_program(VALGRIND_COMMAND valgrind DOC "Valgrind executable")
    set(VALGRIND_FLAGS "--track-origins=yes" CACHE STRING "Flags passed to Valgrind for memcheck targets")
    set(VALGRIND_EXEC_FLAGS "-u Cmdenv" CACHE STRING "Flags passed to executable run by Valgrind")
    string(REPLACE " " ";" _valgrind_flags ${VALGRIND_FLAGS})
    string(REPLACE " " ";" _valgrind_exec_flags ${VALGRIND_EXEC_FLAGS})
    add_custom_target(memcheck_${_name}
        COMMAND ${VALGRIND_COMMAND} ${_valgrind_flags} ${_exec} ${_valgrind_exec_flags} ${_config}
        SOURCES ${_config}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        VERBATIM)
endmacro()

find_package(Vanetza PATHS extern/vanetza/build REQUIRED)
option(Vanetza_STATIC "Link with static Vanetza libraries" OFF)

find_path(Veins_DIR NAMES src/veins/package.ned PATHS extern/veins DOC "Veins root directory")
import_opp_target(veins ${Veins_DIR}/src/Makefile)

find_program(MAKE_COMMAND NAMES make gmake HINTS ${CMAKE_MAKE_COMMAND} DOC "Makefile processor")
add_custom_target(build_veins
    COMMAND ${MAKE_COMMAND} veins
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMENT "Building Veins (external dependency)"
    VERBATIM)
add_dependencies(veins build_veins)

add_subdirectory(src/artery)
add_subdirectory(scenarios)
