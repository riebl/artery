##########
# Sionna #
##########

if(NOT DEFINED ENV{VIRTUAL_ENV})
    message(SEND_ERROR
        "It looks like you are not use virtual environment - which normally means something went wrong. "
        "If you really want to continue, set VIRTUAL_ENV environment variable to system python home."
    )
elseif(NOT IS_DIRECTORY $ENV{VIRTUAL_ENV})
    message(SEND_ERROR "Cannot find virtual environment directory, aborting.")
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(DiscoverCompileOptions)

find_package(Python COMPONENTS Interpreter Development REQUIRED)

find_package(Nanobind MODULE REQUIRED)
find_package(DrJit MODULE REQUIRED)
find_package(Mitsuba MODULE REQUIRED)

add_subdirectory(bridge)
add_subdirectory(environment)

if(WITH_TESTS)
    add_subdirectory(tests)
endif()
