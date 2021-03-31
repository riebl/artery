add_library(artery INTERFACE)
add_library(core SHARED
    StaticNodeManager.cc
    application/CaObject.cc
    application/CaService.cc
    application/DenmObject.cc
    application/DenService.cc
    application/ExampleService.cc
    application/GbcMockMessage.cc
    application/GbcMockService.cc
    application/InfrastructureMockMessage.cc
    application/InfrastructureMockReceiver.cc
    application/InfrastructureMockService.cc
    application/ItsG5BaseService.cc
    application/ItsG5PromiscuousService.cc
    application/ItsG5Service.cc
    application/LocalDynamicMap.cc
    application/LocationTableLogger.cc
    application/Middleware.cc
    application/MultiChannelPolicy.cc
    application/NetworkInterface.cc
    application/NetworkInterfaceTable.cc
    application/PeriodicLoadService.cc
    application/PersonMiddleware.cc
    application/RsuCaService.cc
    application/RtcmMockMessage.cc
    application/RtcmMockReceiver.cc
    application/RtcmMockService.cc
    application/StationType.cc
    application/StationaryMiddleware.cc
    application/StoryboardSignal.cc
    application/Timer.cc
    application/TransportDispatcher.cc
    application/VehicleDataProvider.cc
    application/VehicleKinematics.cc
    application/VehicleMiddleware.cc
    application/XmlMultiChannelPolicy.cc
    application/den/EmergencyBrakeLight.cc
    application/den/ImpactReductionUseCase.cc
    application/den/Memory.cc
    application/den/SuspendableUseCase.cc
    application/den/TractionLoss.cc
    application/den/TrafficJamUseCase.cc
    application/den/UseCase.cc
    networking/AccessInterface.cc
    networking/DccEntityBase.cc
    networking/FsmDccEntity.cc
    networking/GeoNetPacket.cc
    networking/LimericDccEntity.cc
    networking/NoRateControlDccEntity.cc
    networking/PersonPositionProvider.cc
    networking/Router.cc
    networking/Runtime.cc
    networking/SecurityEntity.cc
    networking/StationaryPositionProvider.cc
    networking/VehiclePositionProvider.cc
    nic/ChannelLoadSampler.cc
    nic/RadioDriverBase.cc
    traci/Cast.cc
    traci/MobilityBase.cc
    traci/PersonMobility.cc
    traci/VehicleController.cc
    traci/VehicleMobility.cc
    traci/VehicleType.cc
    utility/AsioScheduler.cc
    utility/AsioTask.cc
    utility/Channel.cc
    utility/Identity.cc
    utility/IdentityRegistry.cc
    utility/FilterRules.cc
    utility/Geometry.cc
)
target_link_libraries(artery INTERFACE core)
add_library(Artery::Core ALIAS core)

generate_opp_message(utility/AsioData.msg TARGET core)

set_target_properties(core PROPERTIES
    OUTPUT_NAME artery_core
    OMNETPP_LIBRARY ON
    NED_FOLDERS ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(core PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_include_directories(core PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(core PUBLIC ${Boost_LIBRARIES})
target_link_libraries(core PUBLIC OmnetPP::envir)
target_link_libraries(core PUBLIC traci)
target_link_libraries(core PUBLIC Vanetza::vanetza)

if(TARGET veins)
    message(STATUS "Enable Veins integration")
    set_property(TARGET core APPEND PROPERTY SOURCES
        veins/VeinsConnectionManager.cc
        veins/VeinsMobility.cc
        veins/VeinsObstacleControl.cc
        veins/VeinsRadioDriver.cc)
    target_link_libraries(core PUBLIC veins)
endif()

if(TARGET INET)
    message(STATUS "Enable INET integration")
    set_property(TARGET core APPEND PROPERTY SOURCES
        inet/AntennaMobility.cc
        inet/ChannelLoadRx.cc
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
        inet/PassiveLogger.cc
        inet/PowerLevelRx.cc
        inet/VanetHcf.cc
        inet/VanetMgmt.cc
        inet/VanetNakagamiFading.cc
        inet/VanetRadio.cc
        inet/VanetReceiver.cc
        inet/VanetRxControl.cc
        inet/VanetTxControl.cc
    )
    target_link_libraries(core PUBLIC INET)
endif()

if(TARGET ots)
    message(STATUS "Enable OpenTrafficSim integration")
    set_property(TARGET core APPEND PROPERTY SOURCES
        ots/GtuMiddleware.cc
        ots/GtuPositionProvider.cc
        ots/GtuProxyService.cc
        ots/GtuInetMobility.cc
    )
    target_link_libraries(core PUBLIC ots)
endif()

if(TARGET lte)
    # SimuLTE is a non-integral feature: add it to "artery" but not "core"
    message(STATUS "Enable SimuLTE integration")
    target_link_libraries(artery INTERFACE lte)
endif()

macro(add_artery_feature name)
    add_library(${name} SHARED ${ARGN})
    add_library(Artery::${name} ALIAS ${name})
    set_target_properties(${name} PROPERTIES
        OUTPUT_NAME artery_${name}
        OMNETPP_LIBRARY ON)
    target_link_libraries(${name} PRIVATE core)
    target_link_libraries(artery INTERFACE Artery::${name})
    install(TARGETS ${name} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
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

install(TARGETS core LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
set_property(TARGET core APPEND PROPERTY INSTALL_NED_FOLDERS ${CMAKE_INSTALL_DATADIR}/ned/artery)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ DESTINATION share/ned/artery FILES_MATCHING PATTERN "*.ned")

get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(NOT is_multi_config)
    generate_run_script(TARGET artery FILE ${PROJECT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/run_artery.sh)
    generate_run_script(TARGET artery FILE ${PROJECT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/run_artery.sh.install INSTALL)
    install(
        PROGRAMS ${PROJECT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/run_artery.sh.install
        DESTINATION ${CMAKE_INSTALL_BINDIR} RENAME run_artery.sh)
endif()
