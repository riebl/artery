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

vanetza_module(dcc ${CXX_SOURCES})
vanetza_intermodule_dependencies(dcc net)

add_test_subdirectory(tests)

