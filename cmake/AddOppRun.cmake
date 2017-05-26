find_program(OMNETPP_RUN NAMES opp_run opp_run_release PATHS ${OMNETPP_ROOT}/bin DOC "OMNeT++ opp_run executable")
include(CMakeParseArguments)

macro(add_opp_run _name)
    set(_one_value_args "CONFIG;DEPENDENCY;WORKING_DIRECTORY")
    set(_multi_value_args "NED_FOLDERS")
    cmake_parse_arguments(_add_opp_run "" "${_one_value_args}" "${_multi_value_args}" ${ARGN})

    if(_add_opp_run_UNPARSED_ARGUMENTS)
        message(SEND_ERROR "add_opp_run called with invalid arguments: ${_add_opp_run_UNPARSED_ARGUMENTS}")
    endif()

    if(_add_opp_run_CONFIG)
        set(_config "${_add_opp_run_CONFIG}")
    else()
        set(_config "omnetpp.ini")
    endif()

    if(_add_opp_run_DEPENDENCY)
        set(_target "${_add_opp_run_DEPENDENCY}")
    else()
        set(_target "artery")
    endif()

    if(_add_opp_run_WORKING_DIRECTORY)
        set(_working_directory "${_add_opp_run_WORKING_DIRECTORY}")
    else()
        set(_working_directory "${CMAKE_CURRENT_SOURCE_DIR}")
    endif()

    get_ned_folders(${_target} _list_ned_folders)
    list(APPEND _list_ned_folders ${_add_opp_run_NED_FOLDERS})
    set(_ned_folders "")
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

    set(RUN_FLAGS "" CACHE STRING "Flags appended to run command (and debug)")
    string(REPLACE " " ";" _run_flags "${RUN_FLAGS}")
    add_custom_target(run_${_name}
        COMMAND ${_exec} ${_config} ${_run_flags}
        WORKING_DIRECTORY ${_working_directory}
        VERBATIM)

    find_program(GDB_COMMAND gdb DOC "GNU debugger")
    if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND GDB_COMMAND)
        add_custom_target(debug_${_name}
            COMMAND ${GDB_COMMAND} --args ${_exec} ${_config} ${_run_flags}
            WORKING_DIRECTORY ${_working_directory}
            VERBATIM)
    endif()

    find_program(VALGRIND_COMMAND valgrind DOC "Valgrind executable")
    set(VALGRIND_FLAGS "--track-origins=yes" CACHE STRING "Flags passed to Valgrind for memcheck targets")
    set(VALGRIND_EXEC_FLAGS "-u Cmdenv" CACHE STRING "Flags passed to executable run by Valgrind")
    string(REPLACE " " ";" _valgrind_flags "${VALGRIND_FLAGS}")
    string(REPLACE " " ";" _valgrind_exec_flags "${VALGRIND_EXEC_FLAGS}")
    add_custom_target(memcheck_${_name}
        COMMAND ${VALGRIND_COMMAND} ${_valgrind_flags} ${_exec} ${_valgrind_exec_flags} ${_config}
        WORKING_DIRECTORY ${_working_directory}
        VERBATIM)
endmacro()
