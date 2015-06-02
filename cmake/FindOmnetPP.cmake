find_path(OMNETPP_ROOT NAMES bin/omnetpp DOC "Path to OMNeT++ root directory")
find_program(OMNETPP_MSGC NAMES nedtool opp_msgc PATHS ${OMNETPP_ROOT}/bin DOC "OMNeT++ message compiler")
find_path(OMNETPP_INCLUDE_DIR NAMES omnetpp.h PATHS ${OMNETPP_ROOT}/include DOC "OMNeT++ include directory")

if(EXISTS ${OMNETPP_ROOT}/Makefile.inc)
    # extract compile definitions from Makefile.inc
    file(STRINGS ${OMNETPP_ROOT}/Makefile.inc _cflags_release REGEX "^CFLAGS_RELEASE = .*")
    string(REGEX MATCHALL "-D[^ ]+" _cflags_release_definitions ${_cflags_release})
    foreach(_cflag_release_definition IN LISTS _cflags_release_definitions)
        string(SUBSTRING ${_cflag_release_definition} 2 -1 _compile_definition)
        list(APPEND _compile_definitions ${_compile_definition})
    endforeach()
endif()

# create imported library targets
set(_libraries cmdenv common envir eventlog layout main nedxml scave sim tkenv)
foreach(_library IN LISTS _libraries)
    string(TOUPPER ${_library} _LIBRARY)
    find_library(OMNETPP_${_LIBRARY}_LIBRARY_RELEASE opp${_library} PATHS ${OMNETPP_ROOT}/lib)
    find_library(OMNETPP_${_LIBRARY}_LIBRARY_DEBUG opp${_library}d PATHS ${OMNETPP_ROOT}/lib)
    add_library(opp_${_library} SHARED IMPORTED)
    list(APPEND OMNETPP_LIBRARIES opp_${_library})
    set_target_properties(opp_${_library} PROPERTIES
        IMPORTED_LOCATION_RELEASE ${OMNETPP_${_LIBRARY}_LIBRARY_RELEASE}
        IMPORTED_LOCATION_DEBUG ${OMNETPP_${_LIBRARY}_LIBRARY_DEBUG}
        INTERFACE_INCLUDE_DIRECTORIES ${OMNETPP_INCLUDE_DIR})
    set_property(TARGET opp_${_library} PROPERTY INTERFACE_COMPILE_DEFINITIONS ${_compile_definitions})
    set_property(TARGET opp_${_library} PROPERTY IMPORTED_CONFIGURATIONS "DEBUG" "RELEASE")
endforeach()
unset(_libraries)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OmnetPP
    FOUND_VAR OMNETPP_FOUND
    REQUIRED_VARS OMNETPP_INCLUDE_DIR OMNETPP_LIBRARIES OMNETPP_MSGC OMNETPP_ROOT)
