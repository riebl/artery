find_path(COHDA_MK2_ROOT
    NAMES ieee1609/app/lib1609/include/lib1609.h
    HINTS ${PROJECT_SOURCE_DIR}/../cohda/mk2
    DOC "Cohda MK2 SDK")

set(COHDA_KERNEL_INCLUDE_DIR ${COHDA_MK2_ROOT}/ieee1609/kernel/include)
set(COHDA_LIB1609_INCLUDE_DIR ${COHDA_MK2_ROOT}/ieee1609/app/lib1609/include)
set(COHDA_INCLUDE_DIRS ${COHDA_KERNEL_INCLUDE_DIR} ${COHDA_LIB1609_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Cohda DEFAULT_MSG COHDA_MK2_ROOT)

