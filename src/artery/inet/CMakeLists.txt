target_sources(core PRIVATE
    AntennaMobility.cc
    ChannelLoadRx.cc
    DistanceSwitchPathLoss.cc
    InetRadioDriver.cc
    InetMobility.cc
    gemv2/LinkClassifier.cc
    gemv2/NLOSb.cc
    gemv2/NLOSf.cc
    gemv2/NLOSv.cc
    gemv2/ObstacleIndex.cc
    gemv2/PathLoss.cc
    gemv2/SmallScaleVariation.cc
    gemv2/VehicleIndex.cc
    gemv2/Visualizer.cc
    PassiveLogger.cc
    PowerLevelRx.cc
    VanetHcf.cc
    VanetMgmt.cc
    VanetNakagamiFading.cc
    VanetRadio.cc
    VanetReceiver.cc
    VanetRxControl.cc
    VanetTxControl.cc
)

target_link_libraries(core PUBLIC INET)
