find_path(PROJ4_INCLUDE_DIR NAMES proj_api.h)
find_library(PROJ4_LIBRARY NAMES proj)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PROJ4 DEFAULT_MSG
    PROJ4_INCLUDE_DIR PROJ4_LIBRARY)

set(PROJ4_INCLUDE_DIRS ${PROJ4_INCLUDE_DIR})
set(PROJ4_LIBRARIES ${PROJ4_LIBRARY})

mark_as_advanced(PROJ4_INCLUDE_DIR PROJ4_LIBRARY)

