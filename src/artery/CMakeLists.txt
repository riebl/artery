add_library(artery INTERFACE)

add_library(core SHARED)
add_library(Artery::Core ALIAS core)

# Artery core modules
add_subdirectory(nic)
add_subdirectory(traci)
add_subdirectory(utility)
add_subdirectory(networking)
add_subdirectory(application)

# TODO: this belongs to utility/, bit until codegen target
# is implemented it should stay here
generate_opp_message(utility/AsioData.msg TARGET core)

target_sources(core PRIVATE
    StaticNodeManager.cc
)

target_link_libraries(artery INTERFACE core)

set_target_properties(core PROPERTIES
    OUTPUT_NAME artery_core
    OMNETPP_LIBRARY ON
    NED_FOLDERS ${CMAKE_CURRENT_SOURCE_DIR}
)

target_include_directories(core PUBLIC
    ${PROJECT_SOURCE_DIR}/src
    ${Boost_INCLUDE_DIRS}
)

target_link_libraries(core PUBLIC
    ${Boost_LIBRARIES}
    OmnetPP::envir
    traci
    Vanetza::vanetza
)

add_artery_subdirectory(veins SWITCH WITH_VEINS)
add_artery_subdirectory(inet SWITCH WITH_INET)
add_artery_subdirectory(storyboard SWITCH WITH_STORYBOARD)

add_artery_subdirectory(transfusion DEPENDENCIES Protobuf SWITCH WITH_TRANSFUSION)
add_artery_subdirectory(testbed DEPENDENCIES SEA_V2X Protobuf SWITCH WITH_TESTBED)

if(TARGET lte)
    # SimuLTE is a non-integral feature: add it to "artery" but not "core"
    message(STATUS "Enable SimuLTE integration")
    target_link_libraries(artery INTERFACE lte)
endif()

if(WITH_ENVMOD)
    add_subdirectory(envmod)
    set_property(SOURCE application/VehicleMiddleware.cc APPEND PROPERTY COMPILE_DEFINITIONS "WITH_ENVMOD")
endif()

install(TARGETS core LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ DESTINATION share/ned/artery FILES_MATCHING PATTERN "*.ned")

set_property(TARGET core APPEND PROPERTY INSTALL_NED_FOLDERS ${CMAKE_INSTALL_DATADIR}/ned/artery)
