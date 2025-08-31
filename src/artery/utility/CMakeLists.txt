target_sources(core PUBLIC
    AsioScheduler.cc
    AsioTask.cc
    Channel.cc
    Identity.cc
    IdentityRegistry.cc
    FilterRules.cc
    Geometry.cc
)

generate_opp_message(MESSAGE AsioData.msg TARGET core)
