set(CXX_SOURCES
    access_control.cpp
    burst_budget.cpp
    channel_load.cpp
    channel_load_smoothing.cpp
    flow_control.cpp
    mapping.cpp
    regular_budget.cpp
    scheduler.cpp
    state_machine.cpp
)

add_vanetza_component(dcc ${CXX_SOURCES})
target_link_libraries(dcc PUBLIC net)

add_test_subdirectory(tests)

