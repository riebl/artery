# DrJit package stub.

include(DiscoverSitePackagesPath)

#####################
# Package Discovery #
#####################

find_package(drjit CONFIG HINTS ${PYTHON_SITE_PACKAGES_PATH}/drjit/cmake REQUIRED)

# DrJit does not provide aliases for imported targets. (for the time of writing this)

if(NOT TARGET drjit::drjit)
    add_library(drjit::drjit ALIAS drjit)
endif()

if(NOT TARGET drjit::drjit-core)
    add_library(drjit::drjit-core ALIAS drjit-core)
endif()

if(NOT TARGET drjit::drjit-extra)
    add_library(drjit::drjit-extra ALIAS drjit-extra)
endif()
