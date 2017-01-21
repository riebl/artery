include(CMakeParseArguments)

#
# find_dependency_components(<dependency> [VERSION <version> [EXACT] COMPONENTS <components...>)
#
# Similar to CMake's find_dependency but with support for components.
#
macro(find_dependency_components _dependency)
    cmake_parse_arguments(_args "EXACT" "VERSION" "COMPONENTS" ${ARGN})
    if(_args_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Invalid argument to find_dependency_components: Unparsed arguments ${_args_UNPARSED_ARGUMENTS}")
    endif()

    if(NOT _args_COMPONENTS)
        message(FATAL_ERROR "Invalid argument to find_dependeny_components: No COMPONENTS given")
    else()
        set(_components ${_args_COMPONENTS})
    endif()

    if(_args_EXACT AND NOT _args_VERSION)
        message(FATAL_ERROR "Invalid argument to find_dependeny_components: EXACT may only be specificed along with VERSION")
    elseif(_args_EXACT)
        set(_version "${_args_VERSION} EXACT")
    else()
        set(_version "${_args_VERSION}")
    endif()

    set(_quiet)
    if(${CMAKE_FIND_PACKAGE_NAME}_FIND_QUIETLY)
        set(_quiet QUIET)
    endif()

    set(_call_find_package False)
    foreach(_component IN LISTS _components)
        if(NOT ${_dependency}_$<UPPER_CASE:${_component}>_FOUND)
            set(_call_find_package True)
        endif()
    endforeach()

    if(_call_find_package)
        if(${CMAKE_FIND_PACKAGE_NAME}_FIND_REQUIRED)
            find_package(${_dependency} ${_version} ${_quiet} COMPONENTS ${_components})
        else()
            find_package(${_dependency} ${_version} ${_quiet} OPTIONAL_COMPONENTS ${_components})
        endif()
    endif()

    if(NOT ${_dependency}_FOUND)
        set(${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE
            "${CMAKE_FIND_PACKAGE_NAME} could not be found because dependency \
            ${_dependency} along with its components could not be found.")
        set(${CMAKE_FIND_PACKAGE_NAME}_FOUND False)
        return()
    endif()
endmacro()
