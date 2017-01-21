include(UseGTest)
configure_gtest_directory(LINK_LIBRARIES net)

add_gtest(ChunkPacket chunk_packet.cpp)
add_gtest(CohesivePacket cohesive_packet.cpp)
add_gtest(MacAddress mac_address.cpp)
add_gtest(OsiLayer osi_layer.cpp)

if(VANETZA_NET_WITH_POSIX)
    add_gtest(EthernetHeader ethernet_header.cpp)
endif()
