# generate sources for messages via opp_msgc
macro(generate_opp_message _msg)
    set(_msg_input messages/${_msg}.msg)
    set(_msg_output
        ${CMAKE_CURRENT_SOURCE_DIR}/messages/${_msg}_m.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/messages/${_msg}_m.h)
    set(_msg_include ${PROJECT_SOURCE_DIR}/src/artery)
    add_custom_command(OUTPUT ${_msg_output}
        COMMAND ${OMNETPP_MSGC} ARGS -s _m.cc -I${_msg_include} ${_msg_input}
        DEPENDS ${_msg_input}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        VERBATIM)
    set_property(DIRECTORY APPEND PROPERTY GENERATED_OPP_MESSAGES ${_msg})
endmacro()

macro(clean_generated_opp_messages)
    execute_process(COMMAND "${OMNETPP_MSGC}" ERROR_VARIABLE _output)
    string(REGEX MATCH "Version: [0-9\.]+, build: [^ ,]+" _opp_msgc_identifier "${_output}")
    if (NOT "${_opp_msgc_identifier}" STREQUAL "${OMNETPP_MSGC_IDENTIFIER}")
        get_property(_generated_messages DIRECTORY PROPERTY GENERATED_OPP_MESSAGES)
        foreach(_message IN LISTS _generated_messages)
            file(REMOVE messages/${_message}_m.cc messages/${_message}_m.h)
        endforeach()
    endif()
    set(OMNETPP_MSGC_IDENTIFIER ${_opp_msgc_identifier} CACHE INTERNAL "identification of OMNeT++ message compiler" FORCE)
endmacro()
variable_watch(OMNETPP_MSGC clean_generated_opp_messages)

generate_opp_message(AsioData)
generate_opp_message(ChannelLoadReport)
generate_opp_message(GeoNetPacket)

add_library(messages OBJECT
    messages/AsioData_m.cc
    messages/ChannelLoadReport_m.cc
    messages/GeoNetPacket_m.cc)
set_target_properties(messages PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    INCLUDE_DIRECTORIES $<TARGET_PROPERTY:core,INCLUDE_DIRECTORIES>)

find_package(Boost COMPONENTS date_time system REQUIRED)

add_library(artery INTERFACE)
add_library(core SHARED
    application/CaObject.cc
    application/CaService.cc
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
    $<TARGET_OBJECTS:messages>
)
target_link_libraries(artery INTERFACE core)
add_library(Artery::Core ALIAS core)

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
        mac/CarrierSensing.cc
        mac/ChannelLoadMeasurements.cc
        mac/Edca.cc
        mac/EdcaQueue.cc
        mac/MacItsG5.cc
        nic/VeinsRadioDriver.cc
        veins/VeinsConnectionManager.cc
        veins/VeinsMobility.cc
        veins/VeinsObstacleControl.cc)
    target_link_libraries(core PUBLIC veins)
endif()

if(TARGET INET)
    message(STATUS "Enable INET integration")
    set_property(TARGET core APPEND PROPERTY SOURCES
        inet/InetRadioDriver.cc
        inet/InetMobility.cc
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

    # DENM service depends on StoryboardSignal
    add_artery_feature(denm
        application/DenmMemory.cc
        application/DenmObject.cc
        application/DenmUseCase.cc
        application/DenmService.cc
        application/ImpactReductionUseCase.cc
        application/TrafficJamUseCase.cc)
    target_link_libraries(denm PUBLIC storyboard)
endif()

if(WITH_TRANSFUSION)
    add_subdirectory(transfusion)
endif()

if(WITH_ENVMOD)
    add_subdirectory(envmod)
    set_property(SOURCE application/VehicleMiddleware.cc APPEND PROPERTY COMPILE_DEFINITIONS "WITH_ENVMOD")
endif()

