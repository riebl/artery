find_package(Cohda MODULE)
if(Cohda_FOUND)
    include_directories(${COHDA_INCLUDE_DIRS})

    find_package(Boost MODULE COMPONENTS system REQUIRED)
    include_directories(${Boost_INCLUDE_DIRS})
    link_libraries(${Boost_SYSTEM_LIBRARY})

    add_executable(cohda_proxy proxy.cpp proxy_handler.cpp)
    target_link_libraries(cohda_proxy net)
endif()
