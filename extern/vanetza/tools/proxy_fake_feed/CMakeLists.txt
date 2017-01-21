if(NOT VANETZA_NET_WITH_POSIX)
    message(STATUS "Skip build of proxy_fake_feed. POSIX extension required.")
    return()
elseif(NOT TARGET Boost::system)
    message(STATUS "Skip build of proxy_fake_feed. Boost::system dependency missing.")
    return()
endif()

add_executable(proxy_fake_feed fake_feed.cpp)
target_link_libraries(proxy_fake_feed Boost::system net)

