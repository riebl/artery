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

    add_library(testbed-protos OBJECT)
    protobuf_generate(
        TARGET testbed-protos
        LANGUAGE cpp
        PROTOS ${CUBE_PROTO_DIR}/nfiniity_cube_radio.proto
        IMPORT_DIRS ${CUBE_PROTO_DIR}
        PROTOC_EXECUTABLE ${Protobuf_PROTOC_EXECUTABLE}
    )
    artery_mark_as_codegen(testbed-protos)

    target_include_directories(testbed PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
    target_link_libraries(testbed PUBLIC testbed-protos)

    if(Protobuf_VERSION VERSION_GREATER_EQUAL "22.0")
        set_property(TARGET testbed PROPERTY CXX_STANDARD 17)
    endif()
else()
    message(STATUS "Building Testbed without CUBE integration")
endif()
