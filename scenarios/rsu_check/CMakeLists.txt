cmake_minimum_required(VERSION 3.19)
    
add_artery_feature(ArteryManager ArteryManager.cc)

target_link_libraries(ArteryManager PUBLIC protobuf::libprotobuf libzmq-static protos comms plog::plog)

add_opp_run(rsu_check CONFIG omnetpp.ini)
