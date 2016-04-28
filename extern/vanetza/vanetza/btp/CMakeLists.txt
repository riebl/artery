set(CXX_SOURCES
    data_indication.cpp
    data_request.cpp
    header.cpp
    port_dispatcher.cpp
)

vanetza_module(btp ${CXX_SOURCES})
vanetza_intermodule_dependencies(btp geonet)

add_test_subdirectory(tests)

