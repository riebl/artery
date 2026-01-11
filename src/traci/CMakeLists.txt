add_library(traci SHARED 
    Defs.cc
    API.cc
    BasicModuleMapper.cc
    BasicNodeManager.cc
    BasicSubscriptionManager.cc
    CheckTimeSync.cc
    Core.cc
    launchers/ConnectLauncher.cc
    launchers/PosixLauncher.cc
    ExtensibleNodeManager.cc
    InsertionDelayVehiclePolicy.cc
    Listener.cc
    MultiTypeModuleMapper.cc
    RegionsOfInterest.cc
    RegionOfInterestVehiclePolicy.cc
    TestbedModuleMapper.cc
    TestbedNodeManager.cc
    ValueUtils.cc
    VariableCache.cc
)

target_link_libraries(traci PUBLIC OmnetPP::envir)
target_include_directories(traci PUBLIC ${CMAKE_SOURCE_DIR}/src)

if(WITH_LIBTRACI)
    target_compile_definitions(traci PUBLIC LIBTRACI)
endif()

set_target_properties(traci PROPERTIES
    NED_FOLDERS ${CMAKE_CURRENT_SOURCE_DIR}
    OMNETPP_LIBRARY ON
)

install(TARGETS traci LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ DESTINATION ${CMAKE_INSTALL_DATADIR}/ned/traci FILES_MATCHING PATTERN "*.ned")

set_property(TARGET traci APPEND PROPERTY INSTALL_NED_FOLDERS ${CMAKE_INSTALL_DATADIR}/ned/traci)

# traci library uses inet/common/ModuleAccess.h
add_dependencies(traci INET)
