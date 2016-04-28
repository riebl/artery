include(UseGTest)
prepare_gtest(net)

add_gtest(ChunkPacket chunk_packet.cpp)
add_gtest(CohesivePacket cohesive_packet.cpp)
add_gtest(MacAddress mac_address.cpp)
add_gtest(OsiLayer osi_layer.cpp)

if(VANETZA_NET_WITH_POSIX)
    set(VANETZA_MODULE_TEST net-posix)
    add_gtest(EthernetHeader ethernet_header.cpp)
endif()
