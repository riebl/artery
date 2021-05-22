find_path(OMNETPP_ROOT NAMES bin/omnetpp PATHS ENV PATH PATH_SUFFIXES .. DOC "Path to OMNeT++ root directory")
find_path(OMNETPP_INCLUDE_DIR NAMES omnetpp.h PATHS ${OMNETPP_ROOT}/include DOC "OMNeT++ include directory")
find_program(OMNETPP_MSGC NAMES nedtool opp_msgc PATHS ${OMNETPP_ROOT}/bin DOC "OMNeT++ message compiler")
find_program(OMNETPP_RUN NAMES opp_run_release opp_run PATHS ${OMNETPP_ROOT}/bin DOC "OMNeT++ opp_run executable")
find_program(OMNETPP_RUNALL NAMES opp_runall PATHS ${OMNETPP_ROOT}/bin DOC "OMNeT++ opp_runall script")
find_program(OMNETPP_RUN_DEBUG NAMES opp_run_dbg opp_run PATHS ${OMNETPP_ROOT}/bin DOC "OMNeT++ opp_run_dbg executable")
get_filename_component(OMNETPP_ROOT "${OMNETPP_ROOT}" REALPATH)
mark_as_advanced(OMNETPP_INCLUDE_DIR OMNETPP_MSGC OMNETPP_ROOT OMNETPP_RUN OMNETPP_RUN_DEBUG)

if(EXISTS ${OMNETPP_ROOT}/Makefile.inc)
    # extract version from Makefile.inc
    file(STRINGS ${OMNETPP_ROOT}/Makefile.inc _inc_version REGEX "^OMNETPP_VERSION =")
    string(REGEX MATCH "([0-9.]+)$" _match_version "${_inc_version}")
    set(OMNETPP_VERSION ${CMAKE_MATCH_1})

    # extract compile definitions from Makefile.inc
    file(STRINGS ${OMNETPP_ROOT}/Makefile.inc _cflags_release REGEX "^CFLAGS_RELEASE =")
    string(REGEX MATCHALL "-D[^ ]+" _cflags_release_definitions "${_cflags_release}")
    foreach(_cflag_release_definition IN LISTS _cflags_release_definitions)
        if (NOT _cflag_release_definition MATCHES "-DNDEBUG=?")
            string(SUBSTRING ${_cflag_release_definition} 2 -1 _compile_definition)
            list(APPEND _compile_definitions ${_compile_definition})
        endif()
    endforeach()
endif()

add_library(opp_header INTERFACE)
target_compile_definitions(opp_header INTERFACE ${_compile_definitions})
target_include_directories(opp_header INTERFACE ${OMNETPP_INCLUDE_DIR})
add_library(OmnetPP::header ALIAS opp_header)

# create imported library targets
set(_libraries cmdenv common envir eventlog layout main nedxml scave sim tkenv)
foreach(_library IN LISTS _libraries)
    string(TOUPPER ${_library} _LIBRARY)
    find_library(OMNETPP_${_LIBRARY}_LIBRARY_RELEASE opp${_library} PATHS ${OMNETPP_ROOT}/lib)
    find_library(OMNETPP_${_LIBRARY}_LIBRARY_DEBUG NAMES opp${_library}d opp${_library}_dbg PATHS ${OMNETPP_ROOT}/lib)
    add_library(OmnetPP::${_library} SHARED IMPORTED)
    list(APPEND OMNETPP_LIBRARIES OmnetPP::${_library})
    set_target_properties(OmnetPP::${_library} PROPERTIES
        IMPORTED_LOCATION_RELEASE ${OMNETPP_${_LIBRARY}_LIBRARY_RELEASE}
        IMPORTED_LOCATION_DEBUG ${OMNETPP_${_LIBRARY}_LIBRARY_DEBUG}
        INTERFACE_LINK_LIBRARIES OmnetPP::header)
    set_property(TARGET OmnetPP::${_library} PROPERTY IMPORTED_CONFIGURATIONS "DEBUG" "RELEASE")
    mark_as_advanced(OMNETPP_${_LIBRARY}_LIBRARY_RELEASE OMNETPP_${_LIBRARY}_LIBRARY_DEBUG)
endforeach()
unset(_libraries)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OmnetPP
    FOUND_VAR OMNETPP_FOUND
    VERSION_VAR OMNETPP_VERSION
    REQUIRED_VARS OMNETPP_INCLUDE_DIR OMNETPP_LIBRARIES OMNETPP_MSGC OMNETPP_ROOT)
