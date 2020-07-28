set(SOURCES
    Angle.cc
    API.cc
    BasicModuleMapper.cc
    BasicNodeManager.cc
    BasicSubscriptionManager.cc
    CheckTimeSync.cc
    Core.cc
    ConnectLauncher.cc
    ExtensibleNodeManager.cc
    InsertionDelayVehiclePolicy.cc
    Listener.cc
    LiteAPI.cc
    MultiTypeModuleMapper.cc
    Position.cc
    PosixLauncher.cc
    RegionsOfInterest.cc
    RegionOfInterestVehiclePolicy.cc
    TestbedModuleMapper.cc
    TestbedNodeManager.cc
    ValueUtils.cc
    VariableCache.cc
    sumo/foreign/tcpip/socket.cpp
    sumo/foreign/tcpip/storage.cpp
    sumo/utils/traci/TraCIAPI.cpp
)

add_library(traci SHARED ${SOURCES})
target_include_directories(traci PUBLIC
    $<TARGET_PROPERTY:core,INCLUDE_DIRECTORIES>
    ${CMAKE_CURRENT_SOURCE_DIR}/sumo)
set_property(TARGET traci PROPERTY NED_FOLDERS ${CMAKE_CURRENT_SOURCE_DIR})
set_property(TARGET traci PROPERTY OMNETPP_LIBRARY ON)

install(TARGETS traci LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ DESTINATION ${CMAKE_INSTALL_DATADIR}/ned/traci FILES_MATCHING PATTERN "*.ned")
set_property(TARGET traci APPEND PROPERTY INSTALL_NED_FOLDERS ${CMAKE_INSTALL_DATADIR}/ned/traci)

# traci library uses inet/common/ModuleAccess.h
add_dependencies(traci INET)
