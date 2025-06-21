add_opp_run(transfusion NED_FOLDERS ${CMAKE_CURRENT_SOURCE_DIR})
add_opp_test(transfusion SUFFIX loopback CONFIG loopback SIMTIME_LIMIT 10s)
