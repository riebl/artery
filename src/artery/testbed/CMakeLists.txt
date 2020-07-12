add_artery_feature(testbed
    GpsdServer.cc
    OtaIndicationQueue.cc
    OtaInterfaceLayer.cc
    OtaInterfaceStub.cc
    TestbedContention.cc
    TestbedRadio.cc
    TestbedScheduler.cc
)

target_link_libraries(testbed PUBLIC INET)

find_package(SEA_V2X CONFIG)
mark_as_advanced(SEA_V2X_DIR)
if (SEA_V2X_FOUND)
    target_sources(testbed PUBLIC
        OtaInterfaceUsrp.cc
        UsrpConnection.cc
    )
    target_link_libraries(testbed PUBLIC SEA_V2X::sea_v2x)
    message(STATUS "Building Testbed with SEA API")
else()
    message(STATUS "Building Testbed without SEA API")
endif()
