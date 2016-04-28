find_package(Boost 1.58 COMPONENTS date_time serialization REQUIRED)

set(CXX_SOURCES
    byte_buffer_convertible.cpp
    byte_buffer_sink.cpp
    byte_buffer_source.cpp
    byte_sequence.cpp
    byte_view.cpp
    clock.cpp
    runtime.cpp
)
vanetza_module(common ${CXX_SOURCES})
vanetza_module_property(common INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS})
vanetza_module_link_libraries(common ${Boost_DATE_TIME_LIBRARY} ${Boost_SERIALIZATION_LIBRARY})

add_test_subdirectory(tests)

