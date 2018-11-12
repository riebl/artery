add_artery_feature(blackice BlackIceCentral.cc BlackIceReporter.cc BlackIceWarner.cc BlackIceWarnerD2D.cc)
generate_opp_message(BlackIceWarning.msg TARGET blackice DIRECTORY lte_msgs)
add_opp_run(lte_blackice DEPENDENCY artery-lte NED_FOLDERS ${CMAKE_CURRENT_SOURCE_DIR})

add_opp_test(lte_blackice SUFFIX backend CONFIG BlackIce-fingerprint-backend)
add_opp_test(lte_blackice SUFFIX peer CONFIG BlackIce-fingerprint-peer)
