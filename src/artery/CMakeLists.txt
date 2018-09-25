find_package(Boost COMPONENTS date_time system REQUIRED)

add_library(artery INTERFACE)
add_library(core SHARED
    application/CaObject.cc
    application/CaService.cc
    application/DenmObject.cc
    application/DenService.cc
    application/ExampleService.cc
    application/Facilities.cc
    application/ItsG5BaseService.cc
    application/ItsG5PromiscuousService.cc
    application/ItsG5Service.cc
    application/LocalDynamicMap.cc
    application/Middleware.cc
    application/StationaryMiddleware.cc
    application/StoryboardSignal.cc
    application/Timer.cc
    application/VehicleDataProvider.cc
    application/VehicleMiddleware.cc
    application/den/ImpactReductionUseCase.cc
    application/den/Memory.cc
    application/den/SuspendableUseCase.cc
    application/den/TrafficJamUseCase.cc
    application/den/UseCase.cc
    netw/GeoNetPacketWrapper.cc
    nic/RadioDriverBase.cc
    traci/MobilityBase.cc
    traci/VehicleController.cc
    traci/VehicleType.cc
    utility/AsioScheduler.cc
    utility/AsioTask.cc
    utility/IdentityRegistry.cc
    utility/FilterRules.cc
    utility/Geometry.cc
)
target_link_libraries(artery INTERFACE core)
add_library(Artery::Core ALIAS core)

generate_opp_message(messages/AsioData.msg TARGET core)
generate_opp_message(messages/ChannelLoadReport.msg TARGET core)
generate_opp_message(messages/GeoNetPacket.msg TARGET core)

set_target_properties(core PROPERTIES
    OUTPUT_NAME artery_core
    OMNETPP_LIBRARY ON
    NED_FOLDERS ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(core PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_include_directories(core PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(core PUBLIC ${Boost_LIBRARIES})
target_link_libraries(core PUBLIC opp_interface)
target_link_libraries(core PUBLIC traci)
target_link_libraries(core PUBLIC Vanetza::vanetza)

if(TARGET veins)
    message(STATUS "Enable Veins integration")
    set_property(TARGET core APPEND PROPERTY SOURCES
        mac/ChannelLoadMeasurements.cc
        nic/VeinsRadioDriver.cc
        veins/VeinsConnectionManager.cc
        veins/VeinsMobility.cc
        veins/VeinsObstacleControl.cc)
    target_link_libraries(core PUBLIC veins)
endif()

if(TARGET INET)
    message(STATUS "Enable INET integration")
    set_property(TARGET core APPEND PROPERTY SOURCES
        inet/AntennaMobility.cc
        inet/DistanceSwitchPathLoss.cc
        inet/InetRadioDriver.cc
        inet/InetMobility.cc
        inet/gemv2/LinkClassifier.cc
        inet/gemv2/NLOSb.cc
        inet/gemv2/NLOSf.cc
        inet/gemv2/NLOSv.cc
        inet/gemv2/ObstacleIndex.cc
        inet/gemv2/PathLoss.cc
        inet/gemv2/SmallScaleVariation.cc
        inet/gemv2/VehicleIndex.cc
        inet/gemv2/Visualizer.cc
        inet/VanetRx.cc)
    target_link_libraries(core PUBLIC INET)
endif()

macro(add_artery_feature name)
    add_library(${name} SHARED ${ARGN})
    add_library(Artery::${name} ALIAS ${name})
    set_target_properties(${name} PROPERTIES
        OUTPUT_NAME artery_${name}
        OMNETPP_LIBRARY ON)
    target_link_libraries(${name} PRIVATE core)
    target_link_libraries(artery INTERFACE Artery::${name})
endmacro()

if(WITH_STORYBOARD)
    add_subdirectory(storyboard)
endif()

if(WITH_TRANSFUSION)
    add_subdirectory(transfusion)
endif()

if(WITH_ENVMOD)
    add_subdirectory(envmod)
    set_property(SOURCE application/VehicleMiddleware.cc APPEND PROPERTY COMPILE_DEFINITIONS "WITH_ENVMOD")
endif()

if(WITH_TESTBED)
    add_subdirectory(testbed)
endif()
