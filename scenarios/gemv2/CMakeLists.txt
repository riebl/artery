add_opp_run(gemv2)

add_opp_test(gemv2 SUFFIX LOS CONFIG LOS_mediumAntennas SIMTIME_LIMIT 10s)
add_opp_test(gemv2 SUFFIX NLOSv CONFIG NLOSv SIMTIME_LIMIT 10s)
add_opp_test(gemv2 SUFFIX NLOSbLogDist CONFIG NLOSb1 SIMTIME_LIMIT 10s)
add_opp_test(gemv2 SUFFIX NLOSbDifRefl CONFIG NLOSb1_diffractionReflection SIMTIME_LIMIT 10s)
add_opp_test(gemv2 SUFFIX NLOSbSmallScale CONFIG NLOSb1_smallScaleVariations SIMTIME_LIMIT 10s)
