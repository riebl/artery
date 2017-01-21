set(CXX_SOURCES
    byte_buffer_convertible.cpp
    byte_buffer_sink.cpp
    byte_buffer_source.cpp
    byte_sequence.cpp
    byte_view.cpp
    clock.cpp
    runtime.cpp
)
add_vanetza_component(common ${CXX_SOURCES})
target_link_libraries(common
    PUBLIC Boost::date_time
    INTERFACE Boost::serialization)

add_test_subdirectory(tests)

