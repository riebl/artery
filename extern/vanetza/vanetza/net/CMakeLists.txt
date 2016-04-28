set(CXX_SOURCES
    access_category.cpp
    buffer_packet.cpp
    chunk_packet.cpp
    cohesive_packet.cpp
    mac_address.cpp
    packet_variant.cpp
    packet.cpp
    proxy_header.cpp
)

set(CXX_SOURCES_POSIX
    ethernet_header.cpp
    io_vector.cpp
    sockaddr.cpp
)

option(VANETZA_NET_WITH_POSIX "Build network module with POSIX extension" ${UNIX})
if(VANETZA_NET_WITH_POSIX)
    list(APPEND CXX_SOURCES ${CXX_SOURCES_POSIX})
endif()

vanetza_module(net ${CXX_SOURCES})
vanetza_intermodule_dependencies(net common)

add_test_subdirectory(tests)

