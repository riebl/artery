add_artery_feature(ros
    ProxyService.cc
    Scheduler.cc
)

macro(find_ros)
    set(ROS_INCLUDE_DIRS "")
    set(ROS_LIBRARIES "")
    foreach(ros_pkg ${ARGV})
        find_package("${ros_pkg}" REQUIRED)
        list(APPEND ROS_INCLUDE_DIRS "${${ros_pkg}_INCLUDE_DIRS}")
        list(APPEND ROS_LIBRARIES "${${ros_pkg}_LIBRARIES}")
    endforeach()
endmacro()

find_ros(etsi_its_msgs roscpp rosgraph_msgs sensor_msgs)
target_include_directories(ros PUBLIC ${ROS_INCLUDE_DIRS})
target_link_libraries(ros PUBLIC ${ROS_LIBRARIES})
