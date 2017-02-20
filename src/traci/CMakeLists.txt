set(SOURCES
    Angle.cc
    API.cc
    BasicModuleMapper.cc
    BasicNodeManager.cc
    Core.cc
    ConnectLauncher.cc
    Listener.cc
    Position.cc
    PosixLauncher.cc
    sumo/foreign/tcpip/socket.cpp
    sumo/foreign/tcpip/storage.cpp
    sumo/utils/traci/TraCIAPI.cpp
)

add_library(traci SHARED ${SOURCES})
target_include_directories(traci PUBLIC
    $<TARGET_PROPERTY:artery,INCLUDE_DIRECTORIES>
    ${CMAKE_CURRENT_SOURCE_DIR}/sumo)
set_property(TARGET traci PROPERTY NED_FOLDERS ${CMAKE_CURRENT_SOURCE_DIR})
