macro(add_opp_build_target _name)
    string(TOLOWER "${_name}" _lower_name)
    set(_build_target "build_${_lower_name}")

    set(_opp_mode "release")
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(_opp_mode "debug")
    endif()

    set(_make_command ${MAKE_COMMAND})
    if(CMAKE_GENERATOR MATCHES "Makefiles")
        set(_make_command \$\(MAKE\))
    endif()

    add_custom_target(${_build_target}
        COMMAND ${_make_command} ${MAKE_ARGUMENTS} ${_lower_name} MODE=${_opp_mode}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMENT "Building ${_name} (external dependency)"
        VERBATIM)
endmacro()
