# Nanobind package stub.

include(DiscoverSitePackagesPath)

#####################
# Package Discovery #
#####################

find_package(nanobind CONFIG HINTS ${PYTHON_SITE_PACKAGES_PATH}/nanobind/cmake REQUIRED)

# Nanobind headers normally are provided via functions, exported by nanobind config file.
# But we have to embed python, so we also provide an interface target that populates headers
# (and other stuff).

if(TARGET nanobind-embed)
    message(FATAL_ERROR "target nanobind-embed is already defined - cannot configure interface target")
endif()

add_library(nanobind-embed INTERFACE)

target_include_directories(nanobind-embed
    INTERFACE
        ${NB_DIR}/include
)

# We make hard assumption here that domain is drjit, I'm not sure its
# possible to validate that embdedding targets will use this exact domain.
target_compile_definitions(nanobind-embed
    INTERFACE
        NB_SHARED
        NB_DOMAIN=drjit
)

if(NOT TARGET nanobind::nanobind-embed)
    add_library(nanobind::nanobind-embed ALIAS nanobind-embed)
endif()
