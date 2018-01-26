add_subdirectory(artery)
if(WITH_STORYBOARD)
add_subdirectory(storyboard)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/mt-its2017/omnetpp.ini)
    add_opp_run(mtits2017 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/mt-its2017)
endif()

if(TARGET lte)
    add_library(artery-lte INTERFACE)
    target_link_libraries(artery-lte INTERFACE artery lte)
    add_opp_run(simulte
        DEPENDENCY artery-lte
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/simulte)
endif()

add_opp_run(car2car-grid WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/car2car-grid)
