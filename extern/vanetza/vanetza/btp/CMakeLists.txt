set(CXX_SOURCES
    data_indication.cpp
    data_request.cpp
    header.cpp
    port_dispatcher.cpp
)

add_vanetza_component(btp ${CXX_SOURCES})
target_link_libraries(btp PUBLIC Boost::boost geonet)

add_test_subdirectory(tests)

