if(NOT TARGET Boost::system)
    message(STATUS "Skip build of socktap because of missing Boost::system dependency")
    return()
endif()

if(NOT TARGET Boost::program_options)
    message(STATUS "Skip build of socktap because of missing Boost::program_options dependency")
    return()
endif()

find_package(Threads REQUIRED)
find_package(GPS REQUIRED)

add_executable(socktap
    application.cpp
    dcc_passthrough.cpp
    ethernet_device.cpp
    gps_position_provider.cpp
    hello_application.cpp
    main.cpp
    router_context.cpp
    time_trigger.cpp
)
target_link_libraries(socktap Boost::system Boost::program_options Threads::Threads GPS::GPS vanetza)
install(TARGETS socktap EXPORT ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})

if(TARGET Cohda::headers)
    set_property(TARGET socktap APPEND PROPERTY COMPILE_DEFINITIONS "SOCKTAP_WITH_COHDA_SDK")
    set_property(TARGET socktap APPEND PROPERTY INCLUDE_DIRECTORIES "${COHDA_INCLUDE_DIRS}")
    set_property(TARGET socktap APPEND PROPERTY SOURCES cohda.cpp)
endif()
