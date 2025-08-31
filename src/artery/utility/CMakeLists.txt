target_sources(core PUBLIC
    AsioScheduler.cc
    AsioTask.cc
    Channel.cc
    Identity.cc
    IdentityRegistry.cc
    FilterRules.cc
    Geometry.cc
)

add_library(asio-data-message OBJECT)
generate_opp_message(MESSAGE AsioData.msg TARGET asio-data-message)

artery_mark_as_codegen(asio-data-message)
target_link_libraries(core PRIVATE asio-data-message)
