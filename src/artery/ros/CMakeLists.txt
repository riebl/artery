add_artery_feature(ros
    ProxyService.cc
    Scheduler.cc
)

find_package(roscpp REQUIRED)
find_package(rosgraph_msgs REQUIRED)

target_include_directories(ros PUBLIC ${roscpp_INCLUDE_DIRS} ${rosgraph_msgs_INCLUDE_DIRS})
target_link_libraries(ros PUBLIC ${roscpp_LIBRARIES} ${rosgraph_msgs_LIBRARIES})
