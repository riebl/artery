find_path(COHDA_ROOT
    NAMES "ieee1609/app/lib1609/include/lib1609.h"
    PATHS "/home/duser"
    PATH_SUFFIXES mk5 mk2
    CMAKE_FIND_ROOT_PATH_BOTH
    DOC "Cohda SDK")

set(COHDA_KERNEL_INCLUDE_DIR ${COHDA_ROOT}/ieee1609/kernel/include)
set(COHDA_LIB1609_INCLUDE_DIR ${COHDA_ROOT}/ieee1609/app/lib1609/include)
set(COHDA_INCLUDE_DIRS ${COHDA_KERNEL_INCLUDE_DIR} ${COHDA_LIB1609_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Cohda "Cohda SDK not found" COHDA_ROOT)
mark_as_advanced(COHDA_ROOT)

if(COHDA_FOUND AND NOT TARGET Cohda::headers)
    add_library(Cohda::headers UNKNOWN IMPORTED)
    set_target_properties(Cohda::headers PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${COHDA_INCLUDE_DIRS}")
endif()
