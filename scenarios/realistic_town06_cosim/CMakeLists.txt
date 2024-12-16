cmake_minimum_required(VERSION 3.19)
    
add_artery_feature(ArteryManager ArteryManager.cc)

target_link_libraries(ArteryManager PRIVATE protobuf::libprotobuf libzmq-static protos comms plog::plog)

add_opp_run(realistic_town06_cosim CONFIG omnetpp.ini)
