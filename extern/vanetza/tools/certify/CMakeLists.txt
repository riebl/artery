if(NOT TARGET Boost::program_options)
    message(STATUS "Skip build of certify because of missing Boost::program_options dependency")
    return()
endif()

add_executable(certify
    commands/auth-ca.cpp
    commands/genkey.cpp
    commands/root-ca.cpp
    commands/show-cert.cpp
    commands/sign-ticket.cpp
    keyio.cpp
    main.cpp
    options.cpp
)

set_target_properties(certify PROPERTIES INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(certify Boost::program_options vanetza)
