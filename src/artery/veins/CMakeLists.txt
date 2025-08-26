target_sources(core PRIVATE
    VeinsConnectionManager.cc
    VeinsMobility.cc
    VeinsObstacleControl.cc
    VeinsRadioDriver.cc
)

target_link_libraries(core PUBLIC veins)
