add_artery_feature(police ClearLaneService.cc PoliceService.cc)

add_opp_message(
    highway-police-message
    MESSAGE PoliceClearLane.msg
    MSG_PREFIX police_msgs
)

target_link_libraries(police PRIVATE highway-police-message)

add_opp_run(highway_police NED_FOLDERS ${CMAKE_CURRENT_SOURCE_DIR})

if(WITH_STORYBOARD)
    target_sources(police PUBLIC PoliceServiceStoryboard.cc)
endif()

if(WITH_ENVMOD)
    target_sources(police PUBLIC PoliceServiceEnvmod.cc)
    target_link_libraries(police PUBLIC envmod)
endif()
