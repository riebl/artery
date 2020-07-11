include(CMakeParseArguments)
if(NOT EXISTS ${OMNETPP_MSGC})
    message(FATAL_ERROR "OMNeT++ message compiler is missing")
endif()

function(add_opp_target)
    set(single_args ROOT_DIR SOURCE_DIR TARGET)
    set(multi_args DEPENDS OPP_MAKEMAKE)
    cmake_parse_arguments(args "" "${single_args}" "${multi_args}" ${ARGN})

    if(NOT args_SOURCE_DIR)
        set(args_SOURCE_DIR ${args_ROOT_DIR}/src)
    endif()

    file(GLOB_RECURSE cpp_files ${args_SOURCE_DIR}/*.cc)
    file(GLOB_RECURSE msg_files ${args_SOURCE_DIR}/*.msg)

    # remove in-tree sources of generated messages
    file(GLOB_RECURSE cpp_msg_files ${args_SOURCE_DIR}/*_m.cc)
    if(cpp_msg_files)
        list(REMOVE_ITEM cpp_files ${cpp_msg_files})
    endif()

    # process opp_makemake options (only -X for now)
    set(exclude_regex "")
    foreach(option IN LISTS args_OPP_MAKEMAKE)
        string(SUBSTRING "${option}" 0 2 option_name)
        string(SUBSTRING "${option}" 2 -1 option_value)
        if (option_name STREQUAL "-X" AND option_value)
            list(APPEND exclude_regex "${args_SOURCE_DIR}/${option_value}")
        endif()
    endforeach()
    string(REPLACE ";" "|" exclude_regex "^(${exclude_regex})")

    # remove excluded source files
    if(NOT exclude_regex STREQUAL "^()")
        foreach(file IN LISTS cpp_files msg_files)
            string(REGEX MATCH "${exclude_regex}" exclude_match ${file})
            if(exclude_match)
                list(APPEND files_excluded ${file})
            endif()
        endforeach()
        if(files_excluded)
            list(REMOVE_ITEM cpp_files ${files_excluded})
            list(REMOVE_ITEM msg_files ${files_excluded})
        endif()
    endif()

    # generate OMNeT++ message code in build directory
    set(msg_gen_dir ${PROJECT_BINARY_DIR}/${args_TARGET}_gen)
    foreach(msg_file IN LISTS msg_files)
        get_filename_component(msg_name "${msg_file}" NAME_WE)
        get_filename_component(msg_dir "${msg_file}" DIRECTORY)
        file(RELATIVE_PATH msg_prefix ${args_SOURCE_DIR} ${msg_dir})
        set(msg_out_dir ${msg_gen_dir}/${msg_prefix})
        file(MAKE_DIRECTORY ${msg_out_dir})
        add_custom_command(OUTPUT "${msg_out_dir}/${msg_name}_m.cc" "${msg_out_dir}/${msg_name}_m.h"
            COMMAND ${OMNETPP_MSGC} ARGS -s _m.cc -h ${msg_file}
            DEPENDS ${msg_file} ${OMNETPP_MSGC}
            COMMENT "Generating message ${msg_prefix}/${msg_name} of ${args_TARGET}"
            WORKING_DIRECTORY ${msg_out_dir} VERBATIM)

        list(APPEND cpp_files "${msg_out_dir}/${msg_name}_m.cc" "${msg_out_dir}/${msg_name}_m.h")
    endforeach()

    # look up NED folders, use source directory as default if no .nedfolders file exists
    set(ned_folders ${args_SOURCE_DIR})
    if(EXISTS "${args_ROOT_DIR}/.nedfolders")
        file(STRINGS "${args_ROOT_DIR}/.nedfolders" ned_folders)
    endif()
    foreach(ned_folder IN LISTS ned_folders)
        get_filename_component(ned_folder_abs ${ned_folder} ABSOLUTE BASE_DIR ${args_ROOT_DIR})
        list(APPEND ned_folders_abs ${ned_folder_abs})
    endforeach()
    
    # set up target for OMNeT++ project
    add_library(${args_TARGET} SHARED ${cpp_files} ${args_DEPENDS})
    target_include_directories(${args_TARGET} PUBLIC ${msg_gen_dir} ${args_SOURCE_DIR})
    target_link_libraries(${args_TARGET} PUBLIC OmnetPP::envir)
    set_property(TARGET ${args_TARGET} PROPERTY LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/extern)
    set_property(TARGET ${args_TARGET} PROPERTY OMNETPP_LIBRARY TRUE)
    set_property(TARGET ${args_TARGET} PROPERTY NED_FOLDERS ${ned_folders_abs})
endfunction()
