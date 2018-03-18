include(CMakeParseArguments)

# generate sources for messages via opp_msgc
macro(generate_opp_message _msg_input)
    cmake_parse_arguments(_gen_opp_msg "" "TARGET;DIRECTORY" "" ${ARGN})
    if(_gen_opp_msg_UNPARSED_ARGUMENTS)
        message(SEND_ERROR "generate_opp_message called with invalid arguments: ${_gen_opp_msg_UNPARSED_ARGUMENTS}")
    endif()

    set(_msg_output_root ${PROJECT_BINARY_DIR}/opp_messages)
    get_filename_component(_msg_name "${_msg_input}" NAME_WE)
    get_filename_component(_msg_dir "${_msg_input}" DIRECTORY)
    if(_gen_opp_msg_DIRECTORY)
        set(_msg_prefix "${_gen_opp_msg_DIRECTORY}")
    else()
        file(RELATIVE_PATH _msg_prefix ${PROJECT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/${_msg_dir})
    endif()
    set(_msg_output_source ${_msg_output_root}/${_msg_prefix}/${_msg_name}_m.cc)
    set(_msg_output_header ${_msg_output_root}/${_msg_prefix}/${_msg_name}_m.h)

    file(MAKE_DIRECTORY ${_msg_output_root}/${_msg_prefix})
    add_custom_command(OUTPUT "${_msg_output_source}" "${_msg_output_header}"
        COMMAND ${OMNETPP_MSGC} ARGS -s _m.cc -h ${CMAKE_CURRENT_SOURCE_DIR}/${_msg_input}
        DEPENDS ${_msg_input} ${OMNETPP_MSGC}
        COMMENT "Generating ${_msg_prefix}/${_msg_name}"
        WORKING_DIRECTORY ${_msg_output_root}/${_msg_prefix} VERBATIM)

    target_sources(${_gen_opp_msg_TARGET} PRIVATE "${_msg_output_source}" "${_msg_output_header}")
    target_include_directories(${_gen_opp_msg_TARGET} PUBLIC ${_msg_output_root})
endmacro()

macro(clean_opp_messages)
    execute_process(COMMAND "${OMNETPP_MSGC}" ERROR_VARIABLE _output)
    string(REGEX MATCH "Version: [0-9\.]+, build: [^ ,]+" _opp_msgc_identifier "${_output}")
    if (NOT "${_opp_msgc_identifier}" STREQUAL "${OMNETPP_MSGC_IDENTIFIER}")
        file(REMOVE_RECURSE ${PROJECT_BINARY_DIR}/opp_messages)
    endif()
    set(OMNETPP_MSGC_IDENTIFIER ${_opp_msgc_identifier} CACHE INTERNAL "identification of OMNeT++ message compiler" FORCE)
endmacro()
variable_watch(OMNETPP_MSGC clean_opp_messages)
