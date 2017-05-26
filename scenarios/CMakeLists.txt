add_subdirectory(artery)
if(WITH_STORYBOARD)
add_subdirectory(storyboard)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/mt-its2017/omnetpp.ini)
    add_opp_run(mtits2017 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/mt-its2017)
endif()
