message(STATUS "Enabled OpenTrafficSim integration")

target_sources(core PRIVATE
    GtuMiddleware.cc
    GtuPositionProvider.cc
    GtuProxyService.cc
    GtuInetMobility.cc
)

target_link_libraries(core PUBLIC ots)
