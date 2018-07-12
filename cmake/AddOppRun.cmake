find_program(OMNETPP_RUN NAMES opp_run_release opp_run PATHS ${OMNETPP_ROOT}/bin DOC "OMNeT++ opp_run executable")
find_program(OMNETPP_RUN_DEBUG NAMES opp_run_dbg opp_run PATHS ${OMNETPP_ROOT}/bin DOC "OMNeT++ opp_run_dbg executable")
include(CMakeParseArguments)

function(_add_opp_run_libraries _target _output)
    set(_libraries "")
    get_target_property(_target_type ${_target} TYPE)
    if (${_target_type} STREQUAL "SHARED_LIBRARY")
        get_target_property(_opp_library ${_target} OMNETPP_LIBRARY)
        if(_opp_library)
            set(_libraries -l $<TARGET_FILE:${_target}> ${_libraries})
        endif()
    endif()

    get_target_property(_interface_libraries ${_target} INTERFACE_LINK_LIBRARIES)
    foreach(_interface_library IN LISTS _interface_libraries)
        if(TARGET ${_interface_library})
            _add_opp_run_libraries(${_interface_library} _libraries_dependency)
            set(_libraries ${_libraries_dependency} ${_libraries})
        endif()
    endforeach()

    set(${_output} ${_libraries} PARENT_SCOPE)
endfunction()

function(_opp_run_command _target _output)
    # collect all NED folders for given target
    get_ned_folders(${_target} _list_ned_folders)
    list(APPEND _list_ned_folders ${_add_opp_run_NED_FOLDERS})
    set(_ned_folders "")
    foreach(_ned_folder IN LISTS _list_ned_folders)
        set(_ned_folders "${_ned_folders}:${_ned_folder}")
    endforeach()
    if(_ned_folders)
        string(SUBSTRING ${_ned_folders} 1 -1 _ned_folders)
    endif()

    # select opp_run binary depending on build type
    if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        set(_opp_run ${OMNETPP_RUN_DEBUG})
    else()
        set(_opp_run ${OMNETPP_RUN})
    endif()

    # build opp_run command depending on target type
    get_target_property(_target_type ${_target} TYPE)
    if(${_target_type} STREQUAL "EXECUTABLE")
        set(_exec $<TARGET_FILE:${_target}> -n ${_ned_folders})
    elseif(${_target_type} STREQUAL "SHARED_LIBRARY")
        set(_exec ${_opp_run} -n ${_ned_folders} -l $<TARGET_FILE:${_target}>)
    else()
        _add_opp_run_libraries(${_target} _opp_libraries)
        set(_exec ${_opp_run} -n ${_ned_folders} ${_opp_libraries})
    endif()

    set(${_output} ${_exec} PARENT_SCOPE)
endfunction()

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

    _opp_run_command(${_target} _exec)

    set(RUN_FLAGS "" CACHE STRING "Flags appended to run command (and debug)")
    string(REPLACE " " ";" _run_flags "${RUN_FLAGS}")
    add_custom_target(run_${_name}
        COMMAND ${_exec} ${_config} ${_run_flags}
        WORKING_DIRECTORY ${_working_directory}
        VERBATIM)

    set_target_properties(run_${_name} PROPERTIES
        OPP_RUN_TARGET ${_target}
        OPP_RUN_CONFIG_FILE ${_config}
        OPP_RUN_WORKING_DIRECTORY ${_working_directory})

    find_program(GDB_COMMAND gdb DOC "GNU debugger")
    if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND GDB_COMMAND)
        add_custom_target(debug_${_name}
            COMMAND ${GDB_COMMAND} --args ${_exec} ${_config} ${_run_flags}
            WORKING_DIRECTORY ${_working_directory}
            VERBATIM)
    endif()

    find_program(VALGRIND_COMMAND valgrind DOC "Valgrind executable")
    if(VALGRIND_COMMAND)
        set(VALGRIND_FLAGS "--track-origins=yes" CACHE STRING "Flags passed to Valgrind for memcheck targets")
        set(VALGRIND_EXEC_FLAGS "-u Cmdenv" CACHE STRING "Flags passed to executable run by Valgrind")
        string(REPLACE " " ";" _valgrind_flags "${VALGRIND_FLAGS}")
        string(REPLACE " " ";" _valgrind_exec_flags "${VALGRIND_EXEC_FLAGS}")
        add_custom_target(memcheck_${_name}
            COMMAND ${VALGRIND_COMMAND} ${_valgrind_flags} ${_exec} ${_valgrind_exec_flags} ${_config}
            WORKING_DIRECTORY ${_working_directory}
            VERBATIM)
    endif()
endmacro()

macro(add_opp_test _name)
    set(_one_value_args "CONFIG;RUN;SIMTIME_LIMIT;SUFFIX")
    set(_multi_value_args "")
    cmake_parse_arguments(_add_opp_test "" "${_one_value_args}" "${_multi_value_args}" ${ARGN})

    if(_add_opp_test_UNPARSED_ARGUMENTS)
        message(SEND_ERROR "add_opp_test called with invalid arguments: ${_add_opp_test_UNPARSED_ARGUMENTS}")
    endif()

    if(_add_opp_test_SUFFIX)
        set(_suffix "${_add_opp_test_SUFFIX}")
    else()
        message(SEND_ERROR "add_opp_test called without required SUFFIX argument")
    endif()

    set(_opp_run_args "-uCmdenv")
    if(_add_opp_test_CONFIG)
        list(APPEND _opp_run_args "-c${_add_opp_test_CONFIG}")
    endif()
    if(_add_opp_test_RUN)
        list(APPEND _opp_run_args "-r${_add_opp_test_RUN}")
    endif()
    if(_add_opp_test_SIMTIME_LIMIT)
        list(APPEND _opp_run_args "--sim-time-limit=${_add_opp_test_SIMTIME_LIMIT}")
    endif()

    get_target_property(_target run_${_name} OPP_RUN_TARGET)
    get_target_property(_working_directory run_${_name} OPP_RUN_WORKING_DIRECTORY)
    get_target_property(_config run_${_name} OPP_RUN_CONFIG_FILE)
    _opp_run_command(${_target} _exec)

    add_test(NAME "${_name}-${_suffix}"
        COMMAND ${_exec} ${_config} ${_opp_run_args}
        WORKING_DIRECTORY ${_working_directory})
endmacro(add_opp_test)
