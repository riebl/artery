add_artery_feature(police ClearLaneService.cc PoliceService.cc PoliceServiceExtended.cc)
generate_opp_message(PoliceClearLane.msg TARGET police DIRECTORY police_msgs)
add_opp_run(highway_police NED_FOLDERS ${CMAKE_CURRENT_SOURCE_DIR})
add_opp_run(highway_police_extended CONFIG omnetppExtended.ini NED_FOLDERS ${CMAKE_CURRENT_SOURCE_DIR})
