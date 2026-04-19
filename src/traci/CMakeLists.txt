###########
# Helpers #
###########

# Add a static library to traci component. This macro
# adds static library to scope, sets include directory
# and adds LIBTRACI compile definition. 
macro(add_traci_component name)
    add_library(${name} STATIC ${ARGN})
    target_include_directories(${name} PUBLIC ${CMAKE_SOURCE_DIR}/src)

    if(WITH_LIBTRACI)
        target_compile_definitions(${name} PUBLIC LIBTRACI)
    endif()
    target_link_libraries(traci PRIVATE ${name})
endmacro()

#########
# TraCI #
#########

add_library(traci SHARED)

# Common code for other components goes here.
add_traci_component(traci_common
    Defs.cc
)

set_target_properties(traci PROPERTIES
    NED_FOLDERS ${CMAKE_CURRENT_SOURCE_DIR}
    OMNETPP_LIBRARY ON
)

install(TARGETS traci LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION ${CMAKE_INSTALL_DATADIR}/ned/traci FILES_MATCHING PATTERN "*.ned")

set_property(TARGET traci APPEND PROPERTY INSTALL_NED_FOLDERS ${CMAKE_INSTALL_DATADIR}/ned/traci)

add_subdirectory(launchers)
