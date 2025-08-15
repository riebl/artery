check_git_submodule(PATH vanetza REQUIRED_FILES CMakeLists.txt)
set(VANETZA_INSTALL ON)

add_subdirectory(vanetza)
mark_as_advanced(
    BUILD_BENCHMARK
    BUILD_CERTIFY
    BUILD_SOCKTAP
    BUILD_TESTS
    BUILD_USING_CONAN
    VANETZA_ASN1_WITH_ASN1C
    VANETZA_EXPORT_PACKAGE
    VANETZA_NET_WITH_POSIX
)

if(WITH_VEINS)
    message(STATUS "Enable Veins integration")
    check_git_submodule(PATH veins REQUIRED_FILES src/veins/package.ned)

    find_path(Veins_DIR NAMES src/veins/package.ned PATHS veins DOC "Veins root directory")
    mark_as_advanced(Veins_DIR)
    add_opp_target(TARGET veins ROOT_DIR ${Veins_DIR})
endif()

if(WITH_INET)
    message(STATUS "Enable INET integration")

    check_git_submodule(PATH inet REQUIRED_FILES src/inet/package.ned)
    find_path(INET_DIR NAMES src/inet/package.ned PATHS inet DOC "INET root directory")
    mark_as_advanced(INET_DIR)

    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${INET_DIR}/.oppfeaturestate ${INET_DIR}/.nedexclusions)

    # collect INET's opp_makemake options
    execute_process(
        COMMAND ./inet_featuretool options -l -f
        WORKING_DIRECTORY ${INET_DIR}
        OUTPUT_VARIABLE INET_OPP_MAKEMAKE
    )
    separate_arguments(INET_OPP_MAKEMAKE UNIX_COMMAND ${INET_OPP_MAKEMAKE})
    
    # generate INET's features and opp_defines header
    set(INET_GEN_DIR ${PROJECT_BINARY_DIR}/INET_gen)
    file(MAKE_DIRECTORY ${INET_GEN_DIR}/inet)

    add_custom_command(
        OUTPUT ${INET_DIR}/.oppfeaturestate
        COMMAND ./inet_featuretool repair
        DEPENDS ${INET_DIR}/.oppfeatures ${INET_DIR}/inet_featuretool
        WORKING_DIRECTORY ${INET_DIR}
    )

    add_custom_command(
        OUTPUT ${INET_GEN_DIR}/inet/features.h
        COMMAND ./inet_featuretool defines >${INET_GEN_DIR}/inet/features.h
        DEPENDS ${INET_DIR}/.oppfeaturestate
        WORKING_DIRECTORY ${INET_DIR}
        COMMENT "Generate INET features.h"
    )

    add_custom_command(
        OUTPUT ${INET_GEN_DIR}/inet/opp_defines.h
        COMMAND ${CMAKE_COMMAND} -E touch ${INET_GEN_DIR}/inet/opp_defines.h
        COMMENT "Generate INET opp_defines.h"
    )
    
    add_opp_target(
        TARGET INET ROOT_DIR ${INET_DIR}
        DEPENDS ${INET_GEN_DIR}/inet/features.h ${INET_GEN_DIR}/inet/opp_defines.h
        OPP_MAKEMAKE ${INET_OPP_MAKEMAKE}
    )
endif()

if(WITH_SIMULTE)
    message(STATUS "Enable SimuLTE integration")
    if(NOT TARGET INET)
        message(FATAL_ERROR "SimuLTE requires INET")
    endif()

    check_git_submodule(PATH simulte REQUIRED_FILES src/package.ned)
    find_path(SimuLTE_DIR NAMES src/package.ned PATHS simulte DOC "SimuLTE root directory")
    mark_as_advanced(SimuLTE_DIR)

    add_opp_target(TARGET lte ROOT_DIR ${SimuLTE_DIR})
    target_link_libraries(lte PUBLIC INET)
else()
    message(STATUS "SimuLTE integration disabled")
endif()

if(WITH_STORYBOARD)
    add_subdirectory(pybind11)
endif()

if(WITH_OTS)
    set(SIM0MQPP_INSTALL ON)
    set(SIM0MQPP_SHARED_LIB ON)
    add_subdirectory(sim0mqpp ${PROJECT_BINARY_DIR}/sim0mqpp)
endif()
