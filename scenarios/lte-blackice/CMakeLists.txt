add_artery_feature(blackice BlackIceCentral.cc BlackIceReporter.cc BlackIceWarner.cc BlackIceWarnerD2D.cc)

add_opp_message(
    lte-blackice-message
    MESSAGE BlackIceWarning.msg
    MSG_PREFIX lte_msgs
)

target_link_libraries(blackice PRIVATE lte-blackice-message)

add_opp_run(lte_blackice DEPENDENCY artery-lte NED_FOLDERS ${CMAKE_CURRENT_SOURCE_DIR})

add_opp_test(lte_blackice SUFFIX backend CONFIG BlackIce-fingerprint-backend)
add_opp_test(lte_blackice SUFFIX peer CONFIG BlackIce-fingerprint-peer)
