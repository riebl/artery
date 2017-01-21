#
# add_vanetza_component(<name> <sources...>)
#
function(add_vanetza_component name)
    set(sources ${ARGN})

    add_library(${name} ${sources})
    target_include_directories(${name} PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>)
    set_target_properties(${name} PROPERTIES
        OUTPUT_NAME vanetza_${name})
    install(TARGETS ${name} EXPORT ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_LIBDIR})
    set_property(GLOBAL APPEND PROPERTY VANETZA_COMPONENTS ${name})
endfunction()

#
# add_test_subdirectory(<directory>)
#
# Add subdirectory only when tests are enabled via BUILD_TESTS
#
function(add_test_subdirectory directory)
  if(BUILD_TESTS)
    add_subdirectory(${directory})
  endif()
endfunction(add_test_subdirectory)
