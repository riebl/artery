target_sources(core PUBLIC
    AsioScheduler.cc
    AsioTask.cc
    Channel.cc
    Identity.cc
    IdentityRegistry.cc
    FilterRules.cc
    Geometry.cc
)

add_opp_message(asio-data-message MESSAGE AsioData.msg)

target_link_libraries(core PRIVATE asio-data-message)
