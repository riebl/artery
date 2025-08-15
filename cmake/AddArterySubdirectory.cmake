function(add_artery_subdirectory directory)
    # include target subdirectory with certain artery
    # extension (feature), if the following conditions match:
    # 1) variable with id matching SWITCH is set to true (feature turned on)
    # 2) packages that are listed in DEPENDENCIES are found
    # 3) targets that are listed in REQUIRES are declared (used for extern modules)

    # If condition 1) is not met, then subdirectory is not added and
    # debug message is issued. If either 2) or 3) fails, then this function raises error.

    set(one_value_args SWITCH)
    set(multi_value_args DEPENDENCIES;REQUIRES)
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
                    "add_artery_subdirectory: dependency '${dependency}' for '${directory}' was not found: variable '${_dep_var}' is not defined"
                )
            endif()
        endforeach()

        foreach(required_target IN LISTS args_REQUIRES)
            if(NOT TARGET ${required_target})
                message(
                    FATAL_ERROR
                    "add_artery_subdirectory: required target '${required_target}' for '${directory}' was not found"
                )
            endif()
        endforeach()

        add_subdirectory("${directory}")
    endif()
endfunction()
