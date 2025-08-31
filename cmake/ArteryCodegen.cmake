# Artery uses a number of instruments that support generating
# C++ source and header files from plain text templates, like
# protobuf or opp_msgc compilers. Often user wants to generate those
# files before running full build (for static code analysis or linting, for example),
# so we introduce special artery-codegen target, which aggregates all code generating
# tasks and runs them together.

set(ARTERY_CODEGEN_TARGET artery-codegen)

if(NOT TARGET ${ARTERY_CODEGEN_TARGET})
    # Codegen target should be always invoked. Custom commands
    # are executed as required - it is a responsibility of the codegen
    # tool provider to ensure sane invocations.
    add_custom_target(${ARTERY_CODEGEN_TARGET} ALL)
endif()

macro(artery_mark_as_codegen target)
    # Marks target as codegen task. Use this on targets that
    # generate source files (and maybe compile them) only.

    if(NOT TARGET ${target})
        message(FATAL_ERROR "artery_mark_as_codegen: '${target}' does not exist")
    endif()
    add_dependencies(${ARTERY_CODEGEN_TARGET} ${target})
endmacro()

function(generate_opp_message)
    # Generate source files for .msg files (Omnet++ custom messages)
    # via opp_msgc compiler. This function generates sources & headers
    # in PREFIX_DIR for specified MESSAGE, then adds them to provided TARGET.
    # Other arguments may be specified via APPEND_OPTIONS. You may include headers
    # by PREFIX_DIR/header.h file later.

    # By default, files are generated under build directory in opp_messages
    # directory. If required, this may be overridden by DIRECTORY argument.

    # If PREFIX_DIR is not specified, prefix will be set relative to src/ directory
    # in current project root.

    set(one_value_args TARGET MESSAGE PREFIX_DIR DIRECTORY)
    set(multi_value_args APPEND_OPTIONS)

    cmake_parse_arguments(args "" "${one_value_args}" "${multi_value_args}" ${ARGN})
    if(args_UNPARSED_ARGUMENTS)
        message(SEND_ERROR "generate_opp_message called with invalid arguments: ${args_UNPARSED_ARGUMENTS}")
    endif()

    set(output_root ${PROJECT_BINARY_DIR}/opp_messages)

    cmake_path(GET args_MESSAGE STEM message_name)
    cmake_path(GET args_MESSAGE PARENT_PATH message_directory)

    if(args_DIRECTORY)
        set(message_prefix "${args_DIRECTORY}")
    else()
        set(absolute_message_path ${CMAKE_CURRENT_SOURCE_DIR}/${msg_dir})
        cmake_path(
            RELATIVE_PATH absolute_message_path
            BASE_DIRECTORY ${PROJECT_SOURCE_DIR}/src
            OUTPUT_VARIABLE message_prefix
        )
    endif()

    set(output_directory "${output_root}/${message_prefix}")
    file(MAKE_DIRECTORY ${output_directory})

    set(message_output_source "${output_directory}/${message_name}_m.cc")
    set(message_output_header "${output_directory}/${message_name}_m.h")

    set(message ${CMAKE_CURRENT_SOURCE_DIR}/${args_MESSAGE})
    add_custom_command(
        OUTPUT "${message_output_source}" "${message_output_header}"
        COMMAND ${OMNETPP_MSGC} ARGS -s _m.cc -h ${message}
        DEPENDS ${args_MESSAGE} ${OMNETPP_MSGC}
        COMMENT "Running opp message compiler on ${message}"
        WORKING_DIRECTORY ${output_directory} VERBATIM
    )

    target_sources(${args_TARGET} PRIVATE ${message_output_source} ${message_output_header})
    target_link_libraries(${args_TARGET} PUBLIC OmnetPP::header)
    target_include_directories(${args_TARGET} PUBLIC ${output_root})
endfunction()
