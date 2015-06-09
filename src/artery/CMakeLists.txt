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

generate_opp_msg(ChannelLoadReport)
generate_opp_msg(GeoNetPacket)

find_package(Boost COMPONENTS date_time REQUIRED)

set(SOURCES
    application/CaService.cc
    application/ExampleService.cc
    application/Facilities.cc
    application/ItsG5BaseService.cc
    application/ItsG5Middleware.cc
    application/ItsG5Service.cc
    application/VehicleDataProvider.cc
    mac/CarrierSensing.cc
    mac/ChannelLoadMeasurements.cc
    mac/Edca.cc
    mac/EdcaQueue.cc
    mac/MacItsG5.cc
    messages/ChannelLoadReport_m.cc
    messages/GeoNetPacket_m.cc
    netw/GeoNetPacketWrapper.cc
)

add_library(artery SHARED ${SOURCES})
set_property(TARGET artery PROPERTY NED_FOLDERS ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(artery PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(artery opp_interface)
target_link_libraries(artery veins)
target_link_libraries(artery Vanetza::asn1 Vanetza::btp Vanetza::geonet)
target_link_libraries(artery ${Boost_LIBRARIES})
target_include_directories(artery PUBLIC ${Boost_INCLUDE_DIRS})
