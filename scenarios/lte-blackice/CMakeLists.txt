add_artery_feature(blackice BlackIceCentral.cc BlackIceReporter.cc BlackIceWarner.cc)
generate_opp_message(BlackIceWarning.msg TARGET blackice DIRECTORY lte_msgs)
add_opp_run(lte_blackice DEPENDENCY artery-lte NED_FOLDERS ${CMAKE_CURRENT_SOURCE_DIR})
