add_opp_run(storyboard)
add_opp_test(storyboard SUFFIX inet SIMTIME_LIMIT 60s)
add_opp_test(storyboard SUFFIX veins CONFIG veins SIMTIME_LIMIT 60s)
