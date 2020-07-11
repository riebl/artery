include(CMakeParseArguments)

# generate sources for messages via opp_msgc
function(generate_opp_message msg_input)
    cmake_parse_arguments(args "" "TARGET;DIRECTORY" "" ${ARGN})
    if(args_UNPARSED_ARGUMENTS)
        message(SEND_ERROR "generate_opp_message called with invalid arguments: ${args_UNPARSED_ARGUMENTS}")
    endif()

    set(msg_output_root ${PROJECT_BINARY_DIR}/opp_messages)
    get_filename_component(msg_name "${msg_input}" NAME_WE)
    get_filename_component(msg_dir "${msg_input}" DIRECTORY)
    if(args_DIRECTORY)
        set(msg_prefix "${args_DIRECTORY}")
    else()
        file(RELATIVE_PATH msg_prefix ${PROJECT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/${msg_dir})
    endif()
    set(msg_output_dir "${msg_output_root}/${msg_prefix}")
    set(msg_output_source "${msg_output_dir}/${msg_name}_m.cc")
    set(msg_output_header "${msg_output_dir}/${msg_name}_m.h")

    file(MAKE_DIRECTORY ${msg_output_dir})
    add_custom_command(OUTPUT "${msg_output_source}" "${msg_output_header}"
        COMMAND ${OMNETPP_MSGC} ARGS -s _m.cc -h ${CMAKE_CURRENT_SOURCE_DIR}/${msg_input}
        DEPENDS ${msg_input} ${OMNETPP_MSGC}
        COMMENT "Generating ${msg_prefix}/${msg_name}"
        WORKING_DIRECTORY ${msg_output_dir} VERBATIM)

    target_sources(${args_TARGET} PRIVATE "${msg_output_source}" "${msg_output_header}")
    target_include_directories(${args_TARGET} PUBLIC ${msg_output_root})
endfunction()
