add_artery_feature(police ClearLaneService.cc PoliceService.cc)

add_library(highway-police-message OBJECT)
generate_opp_message(MESSAGE PoliceClearLane.msg TARGET highway-police-message DIRECTORY police_msgs)

artery_mark_as_codegen(highway-police-message)
target_link_libraries(police PRIVATE highway-police-message)

add_opp_run(highway_police NED_FOLDERS ${CMAKE_CURRENT_SOURCE_DIR})

if(WITH_STORYBOARD)
    target_sources(police PUBLIC PoliceServiceStoryboard.cc)
endif()

if(WITH_ENVMOD)
    target_sources(police PUBLIC PoliceServiceEnvmod.cc)
    target_link_libraries(police PUBLIC envmod)
endif()
