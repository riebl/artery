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
    get_property(_generated_messages DIRECTORY PROPERTY GENERATED_OPP_MESSAGES)
    foreach(_message IN LISTS _generated_messages)
        file(REMOVE messages/${_message}_m.cc messages/${_message}_m.h)
    endforeach()
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
    INCLUDE_DIRECTORIES $<TARGET_PROPERTY:artery,INCLUDE_DIRECTORIES>)

find_package(Boost COMPONENTS date_time system REQUIRED)

add_library(artery SHARED
    application/CaObject.cc
    application/CaService.cc
    application/ExampleService.cc
    application/Facilities.cc
    application/ItsG5BaseService.cc
    application/ItsG5Middleware.cc
    application/ItsG5PromiscuousService.cc
    application/ItsG5Service.cc
    application/LocalDynamicMap.cc
    application/Timer.cc
    application/VehicleDataProvider.cc
    mac/CarrierSensing.cc
    mac/ChannelLoadMeasurements.cc
    mac/Edca.cc
    mac/EdcaQueue.cc
    mac/MacItsG5.cc
    netw/GeoNetPacketWrapper.cc
    nic/RadioDriverBase.cc
    nic/VeinsRadioDriver.cc
    traci/MobilityBase.cc
    traci/VehicleController.cc
    traci/VehicleType.cc
    utility/AsioScheduler.cc
    utility/AsioTask.cc
    utility/IdentityRegistry.cc
    utility/FilterRules.cc
    utility/Geometry.cc
    veins/VeinsMobility.cc
    veins/VeinsObstacleControl.cc
    $<TARGET_OBJECTS:messages>
)

set_property(TARGET artery PROPERTY NED_FOLDERS ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(artery PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_include_directories(artery PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(artery PUBLIC ${Boost_LIBRARIES})
target_link_libraries(artery PUBLIC opp_interface)
target_link_libraries(artery PUBLIC traci)
target_link_libraries(artery PUBLIC veins)
target_link_libraries(artery PUBLIC Vanetza::vanetza)

if(TARGET INET)
    message(STATUS "Enable INET integration")
    set_property(TARGET artery APPEND PROPERTY SOURCES
        inet/InetRadioDriver.cc
        inet/InetMobility.cc
        inet/VanetRx.cc)
    target_link_libraries(artery PUBLIC INET)
endif()

macro(add_artery_feature name)
    add_library(${name} SHARED ${ARGN})
    set_target_properties(${name} PROPERTIES
        INCLUDE_DIRECTORIES $<TARGET_PROPERTY:artery,INCLUDE_DIRECTORIES>)
endmacro()

if(WITH_STORYBOARD)
    add_subdirectory(storyboard)
    target_link_libraries(artery PRIVATE storyboard)

    # DENM service depends on StoryboardSignal
    add_artery_feature(denm
        application/DenmMemory.cc
        application/DenmObject.cc
        application/DenmUseCase.cc
        application/DenmService.cc
        application/ImpactReductionUseCase.cc
        application/TrafficJamUseCase.cc)
    target_link_libraries(artery PRIVATE denm)
endif()

if(WITH_TRANSFUSION)
    add_subdirectory(transfusion)
    target_link_libraries(artery PRIVATE transfusion)
endif()


if("${Vanetza_DIR}" STREQUAL "${PROJECT_SOURCE_DIR}/extern/vanetza/build")
    set(_make_command ${MAKE_COMMAND})
    if(CMAKE_GENERATOR MATCHES "Makefiles")
        set(_make_command \$\(MAKE\))
    endif()
    add_custom_target(build_vanetza
        COMMAND ${_make_command} ${MAKE_ARGUMENTS} vanetza
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMENT "Building Vanetza (external dependency)"
        VERBATIM)
    add_dependencies(artery build_vanetza)
endif()
