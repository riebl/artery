include(CMakeParseArguments)

function(check_git_submodule)
    set(single_args PATH)
    set(multi_args REQUIRED_FILES)
    cmake_parse_arguments(args "" "${single_args}" "${multi_args}" ${ARGN})

    set(missing_files "")
    foreach(required_file IN LISTS args_REQUIRED_FILES)
        if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${args_PATH}/${required_file})
            list(APPEND missing_files ${required_file})
        endif()
    endforeach()

    if(missing_files)
        string(REPLACE ";" ", " missing_files "${missing_files}")
        message(FATAL_ERROR
            "Git submodule at ${args_PATH} is incomplete (missing: ${missing_files})! Please fetch git submodules and try again.")
    endif()
endfunction()
