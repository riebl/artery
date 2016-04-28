if(NOT VANETZA_NET_WITH_POSIX)
    return()
endif()

find_package(Boost MODULE COMPONENTS system REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
link_libraries(${Boost_SYSTEM_LIBRARY})

add_executable(proxy_fake_feed fake_feed.cpp)
target_link_libraries(proxy_fake_feed net)

