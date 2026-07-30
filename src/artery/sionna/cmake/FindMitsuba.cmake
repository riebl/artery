# Mitsuba3 exports shared libraries and even headers when installed from python
# package registry.

include(DiscoverSitePackagesPath)
include(FindPackageHandleStandardArgs)

#####################
# Package Discovery #
#####################

set(_mi_root ${PYTHON_SITE_PACKAGES_PATH}/mitsuba)

find_path(MITSUBA_INCLUDE_DIRS
    NAMES mitsuba/mitsuba.h
    HINTS
        ${_mi_root}
        ${_mi_root}/include
    NO_DEFAULT_PATH
)

# Extract version macros from mitsuba.h, if requested.

set(_mitsuba_header "${MITSUBA_INCLUDE_DIRS}/mitsuba/mitsuba.h")

if(PACKAGE_FIND_VERSION_COUNT GREATER 0)
    if(NOT EXISTS "${_mitsuba_header}")
        message(FATAL_ERROR "Cannot find mitsuba.h at ${_mitsuba_header} - version not detected")
    endif()
endif()

if(PACKAGE_FIND_VERSION_MAJOR)
    file(STRINGS "${_mitsuba_header}" _mi_major_line REGEX "^#define[\\s]+MI_VERSION_MAJOR[\\s]+[0-9]+")
    string(REGEX MATCH "[0-9]+" Mitsuba_VERSION_MAJOR "${_mi_major_line}")
endif()

if(PACKAGE_FIND_VERSION_MINOR)
    file(STRINGS "${_mitsuba_header}" _mi_minor_line REGEX "^#define[ \t]+MI_VERSION_MINOR[ \t]+[0-9]+")
    string(REGEX MATCH "[0-9]+" Mitsuba_VERSION_MINOR "${_mi_minor_line}")
endif()

if(PACKAGE_FIND_VERSION_PATCH)
    file(STRINGS "${_mitsuba_header}" _mi_patch_line REGEX "^#define[ \t]+MI_VERSION_PATCH[ \t]+[0-9]+")
    string(REGEX MATCH "[0-9]+" Mitsuba_VERSION_PATCH "${_mi_patch_line}")
endif()

if(PACKAGE_FIND_VERSION)
    set(Mitsuba_VERSION "${Mitsuba_VERSION_MAJOR}.${Mitsuba_VERSION_MINOR}.${Mitsuba_VERSION_PATCH}")
    message(STATUS "Detected Mitsuba version: ${Mitsuba_VERSION}")
endif()

set(MITSUBA_LIBRARIES )
set(_libraries mitsuba nanobind-drjit)

foreach(_library IN ITEMS ${_libraries})
    unset(_library_path CACHE)
    find_library(_library_path NAMES ${_library} HINTS ${_mi_root} NO_DEFAULT_PATH REQUIRED)

    set(_library_name mitsuba-${_library})
    set(_library_alias Mitsuba::${_library})

    add_library(${_library_name} SHARED IMPORTED)
    add_library(${_library_alias} ALIAS ${_library_name})

    target_include_directories(${_library_name} INTERFACE ${MITSUBA_INCLUDE_DIRS})
    target_link_libraries(${_library_name}
        INTERFACE
            nanobind::nanobind-embed
            drjit::drjit
    )

    set_target_properties(${_library_name}
        PROPERTIES
            IMPORTED_NO_SONAME ON
            IMPORTED_LOCATION ${_library_path}
    )

    list(APPEND MITSUBA_LIBRARIES ${_library_name})
endforeach()

if(DEFINED Mitsuba_VERSION)
    find_package_handle_standard_args(Mitsuba
        REQUIRED_VARS MITSUBA_INCLUDE_DIRS MITSUBA_LIBRARIES
        VERSION_VAR Mitsuba_VERSION
    )
else()
    find_package_handle_standard_args(Mitsuba
        REQUIRED_VARS MITSUBA_INCLUDE_DIRS MITSUBA_LIBRARIES
    )
endif()
