add_artery_feature(
    testbed
    GpsdServer.cc
    OtaIndicationQueue.cc
    OtaInterfaceLayer.cc
    OtaInterfaceStub.cc
    TestbedContention.cc
    TestbedRadio.cc
    TestbedScheduler.cc
)

target_link_libraries(testbed PUBLIC INET)

if(SEA_V2X_FOUND)
    message(STATUS "Building Testbed with SEA API")
    target_sources(testbed PUBLIC OtaInterfaceUsrp.cc UsrpConnection.cc)
    target_link_libraries(testbed PUBLIC SEA_V2X::sea_v2x)
else()
    message(STATUS "Building Testbed without SEA API")
endif()

if(Protobuf_FOUND)
    message(STATUS "Building Testbed with CUBE integration")
    set(CUBE_PROTO_DIR ${PROJECT_SOURCE_DIR}/extern/vanetza/tools/socktap)

    target_sources(
        testbed PUBLIC
        CubeConnection.cc
        OtaInterfaceCube.cc
    )

    protobuf_generate_cpp(
        TESTBED_PROTO_SRCS TESTBED_PROTO_HDRS
        ${CUBE_PROTO_DIR}/nfiniity_cube_radio.proto
    )

    add_library(
        testbed-protos OBJECT
        ${TESTBED_PROTO_SRCS} ${TESTBED_PROTO_HDRS}
    )

    artery_mark_as_codegen(testbed-protos)

    target_link_libraries(testbed-protos PUBLIC ${PROTOBUF_LIBRARIES})

    target_include_directories(
        testbed-protos PUBLIC
        ${PROTOBUF_INCLUDE_DIRS}
        ${CMAKE_CURRENT_BINARY_DIR}
    )

    target_link_libraries(testbed PUBLIC testbed-protos)

    if(Protobuf_VERSION VERSION_GREATER_EQUAL "22.0")
        set_property(TARGET testbed PROPERTY CXX_STANDARD 17)
    endif()
else()
    message(STATUS "Building Testbed without CUBE integration")
endif()
