function(add_artery_subdirectory directory)
    set(one_value_args SWITCH)
    set(multi_value_args DEPENDENCIES)
    cmake_parse_arguments(args "" "${one_value_args}" "${multi_value_args}" ${ARGN})

    if(args_UNPARSED_ARGUMENTS)
        message(SEND_ERROR "add_artery_subdirectory called with invalid arguments: ${args_UNPARSED_ARGUMENTS}")
    endif()

    if(NOT ${${args_SWITCH}})
        message(STATUS "add_artery_subdirectory: skipping ${directory} (switched off)")
    else()
        message(STATUS "add_artery_subdirectory: adding ${directory} (switched on)")

        foreach(dependency IN LISTS args_DEPENDENCIES)
            set(_dep_var "${dependency}_FOUND")
            if(NOT DEFINED ${_dep_var} OR NOT ${_dep_var})
                message(
                    FATAL_ERROR
                    "add_artery_subdirectory: required dependency '${dependency}' for '${directory}' not found: variable '${_dep_var}' is not defined"
                )
            endif()
        endforeach()

        add_subdirectory("${directory}")
    endif()
endfunction()
