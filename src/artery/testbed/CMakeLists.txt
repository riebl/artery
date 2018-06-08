add_artery_feature(testbed
    OtaIndicationQueue.cc
    OtaInterfaceLayer.cc
    TestbedContention.cc
    TestbedRadio.cc
    TestbedScheduler.cc
)

target_link_libraries(testbed PUBLIC INET)
