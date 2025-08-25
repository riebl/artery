function(add_artery_subdirectory directory)
    # include target subdirectory with certain artery
    # extension (feature), if the following conditions match:
    # 1) variable with id matching SWITCH is set to true (feature turned on)
    # 2) targets that are listed in REQUIRES are declared (exist by their provided names)

    # If condition 1) is not met, then subdirectory is not added and
    # debug message is issued. If one or more targets specified by REQUIRES does not exist,
    # this function raises error. Note: these checks are not meant to determine if package
    # was loaded correctly, but rather to make feature-to-package relation verbose in CMake files.

    set(one_value_args SWITCH)
    set(multi_value_args REQUIRES)
    cmake_parse_arguments(args "" "${one_value_args}" "${multi_value_args}" ${ARGN})

    if(args_UNPARSED_ARGUMENTS)
        message(SEND_ERROR "add_artery_subdirectory called with invalid arguments: ${args_UNPARSED_ARGUMENTS}")
    endif()

    if(NOT ${${args_SWITCH}})
        message(STATUS "add_artery_subdirectory: skipping ${directory} (switched off)")
    else()
        message(STATUS "add_artery_subdirectory: adding ${directory} (switched on)")

        foreach(required_target IN LISTS args_REQUIRES)
            if(NOT TARGET ${required_target})
                message(
                    FATAL_ERROR
                    "add_artery_subdirectory: required target '${required_target}' for '${directory}' does not exist"
                )
            endif()
        endforeach()

        add_subdirectory("${directory}")
    endif()
endfunction()
