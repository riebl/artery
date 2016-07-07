# generate sources for messages via opp_msgc
macro(generate_opp_msg _msg)
    set(_msg_input ${CMAKE_CURRENT_SOURCE_DIR}/messages/${_msg}.msg)
    set(_msg_output ${CMAKE_CURRENT_SOURCE_DIR}/messages/${_msg}_m.cc)
    set(_msg_include ${PROJECT_SOURCE_DIR}/src/artery)
    add_custom_command(OUTPUT ${_msg_output}
        COMMAND ${OMNETPP_MSGC} -s _m.cc -I${_msg_include} ${_msg_input}
        DEPENDS ${_msg_input}
        VERBATIM
    )
endmacro()

generate_opp_msg(AsioData)
generate_opp_msg(ChannelLoadReport)
generate_opp_msg(GeoNetPacket)

add_library(messages OBJECT
    messages/AsioData_m.cc
    messages/ChannelLoadReport_m.cc
    messages/GeoNetPacket_m.cc)
set_target_properties(messages PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    INCLUDE_DIRECTORIES $<TARGET_PROPERTY:artery,INCLUDE_DIRECTORIES>)

find_package(Boost COMPONENTS date_time system REQUIRED)

set(SOURCES
    application/CaService.cc
    application/DenmObject.cc
    application/DenmService.cc
    application/ExampleService.cc
    application/Facilities.cc
    application/ItsG5BaseService.cc
    application/ItsG5Middleware.cc
    application/ItsG5PromiscuousService.cc
    application/ItsG5Service.cc
    application/Timer.cc
    application/VehicleDataProvider.cc
    mac/CarrierSensing.cc
    mac/ChannelLoadMeasurements.cc
    mac/Edca.cc
    mac/EdcaQueue.cc
    mac/MacItsG5.cc
    netw/GeoNetPacketWrapper.cc
    traci/TraCIArteryNodeManager.cc
    traci/TraCIScenarioManagerArtery.cc
    utility/AsioScheduler.cc
    utility/AsioTask.cc
)

if(TARGET INET)
    message(STATUS "Enable INET integration")
    list(APPEND SOURCES
        inet/InetTraCIMobility.cc
    )
endif()

add_library(artery SHARED ${SOURCES} $<TARGET_OBJECTS:messages>)
set_property(TARGET artery PROPERTY NED_FOLDERS ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(artery PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(artery opp_interface)
target_link_libraries(artery veins)
target_link_libraries(artery Vanetza::vanetza)
target_link_libraries(artery ${Boost_LIBRARIES})
target_include_directories(artery PUBLIC ${Boost_INCLUDE_DIRS})

if(TARGET transfusion)
    target_link_libraries(artery transfusion)
    add_dependencies(transfusion messages)
endif()

if(TARGET INET)
    target_link_libraries(artery INET)
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
