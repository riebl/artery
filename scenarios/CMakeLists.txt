add_subdirectory(artery)
add_subdirectory(gemv2)
add_subdirectory(highway-police)
add_subdirectory(single_town06_cosim)
add_subdirectory(transfusion)

add_opp_run(realistic_town06_cosim WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/realistic_town06_cosim NED_FOLDERS ${CMAKE_SOURCE_DIR}/src/cavise)
add_opp_run(rsu_check WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/rsu_check NED_FOLDERS ${CMAKE_SOURCE_DIR}/src/cavise)
add_opp_run(2cars_2rsu_coperception WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/2cars_2rsu_coperception NED_FOLDERS ${CMAKE_SOURCE_DIR}/src/cavise)
add_opp_run(car_rsu_traffic_coperception WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/car_rsu_traffic_coperception NED_FOLDERS ${CMAKE_SOURCE_DIR}/src/cavise)
add_opp_run(3cars_coperception WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/3cars_coperception NED_FOLDERS ${CMAKE_SOURCE_DIR}/src/cavise)
add_opp_run(2cars_no_rsu_coperception WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/2cars_no_rsu_coperception NED_FOLDERS ${CMAKE_SOURCE_DIR}/src/cavise)
add_opp_run(cooperative_intersection WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/cooperative_intersection NED_FOLDERS ${CMAKE_SOURCE_DIR}/src/cavise)
add_opp_run(4cars_coperception WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/4cars_coperception NED_FOLDERS ${CMAKE_SOURCE_DIR}/src/cavise)

if(WITH_ENVMOD)
    add_subdirectory(rsu_grid)
endif()
if(WITH_STORYBOARD)
    add_subdirectory(storyboard)
endif()
if(WITH_TESTBED)
    add_subdirectory(testbed)
endif()

if(WITH_OTS)
    add_opp_run(ots_demo WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ots-demo)
endif()


if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/mt-its2017/omnetpp.ini AND WITH_STORYBOARD)
    add_opp_run(mtits2017 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/mt-its2017)
    add_opp_test(mtits2017 SUFFIX test SIMTIME_LIMIT 30s)
endif()

if(TARGET lte)
    # keep artery-lte target for backward-compatibility
    add_library(artery-lte ALIAS artery)
    add_opp_run(simulte WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/simulte)
    add_opp_test(simulte SUFFIX cellular CONFIG VoIP-UpLink SIMTIME_LIMIT 10s)
    add_opp_test(simulte SUFFIX d2d CONFIG AlertMulticast SIMTIME_LIMIT 25s)
endif()

if(TARGET lte AND WITH_STORYBOARD)
    add_subdirectory(lte-blackice)
endif()

add_opp_run(car2car-grid WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/car2car-grid)
add_opp_test(car2car-grid SUFFIX cam_bsp CONFIG cam_bsp RUN 0 SIMTIME_LIMIT 30s)
