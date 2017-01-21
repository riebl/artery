set(CXX_SOURCES
    cam_functions.cpp
    path_history.cpp
    path_point.cpp
)

add_vanetza_component(facilities ${CXX_SOURCES})
target_link_libraries(facilities PUBLIC Boost::date_time asn1)

add_test_subdirectory(tests)

