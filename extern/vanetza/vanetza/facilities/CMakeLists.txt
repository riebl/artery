find_package(Boost COMPONENTS date_time REQUIRED)

set(CXX_SOURCES
    cam_functions.cpp
    path_history.cpp
    path_point.cpp
)

vanetza_module(facilities ${CXX_SOURCES})
vanetza_intermodule_dependencies(facilities asn1)

vanetza_module_property(facilities INCLUDE_DIRECTORIES
    ${Boost_INCLUDE_DIRS}
)
vanetza_module_link_libraries(facilities
    ${Boost_DATE_TIME_LIBRARY}
)

add_test_subdirectory(tests)

