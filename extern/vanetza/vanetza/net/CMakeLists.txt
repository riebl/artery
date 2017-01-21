set(CXX_SOURCES
    access_category.cpp
    buffer_packet.cpp
    chunk_packet.cpp
    cohesive_packet.cpp
    ethernet_header.cpp
    mac_address.cpp
    packet_variant.cpp
    packet.cpp
    proxy_header.cpp
)

set(CXX_SOURCES_POSIX
    io_vector.cpp
    sockaddr.cpp
)

option(VANETZA_NET_WITH_POSIX "Build network module with POSIX extension" ${UNIX})
if(VANETZA_NET_WITH_POSIX)
    list(APPEND CXX_SOURCES ${CXX_SOURCES_POSIX})
endif()

add_vanetza_component(net ${CXX_SOURCES})
target_link_libraries(net PUBLIC common)

add_test_subdirectory(tests)

