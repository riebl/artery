# Prepare GTest environment and create MODULE_test interface library
## MODULE module under test
## SRCS... optional list of sources to link with each subsequent gtest
macro(prepare_gtest MODULE)
  add_library(${MODULE}_test INTERFACE)
  target_link_libraries(${MODULE}_test INTERFACE ${MODULE})
  set(VANETZA_GTEST_LINK_LIBRARIES ${MODULE}_test)
  if("${ARGC}" GREATER "1")
    add_library(${MODULE}_test_objs OBJECT ${ARGN})
    get_target_property(_module_inc_dirs ${MODULE} INTERFACE_INCLUDE_DIRECTORIES)
    target_include_directories(${MODULE}_test_objs PUBLIC ${_module_inc_dirs} ${GTest_INCLUDE_DIR})
    set(VANETZA_GTEST_OBJECTS $<TARGET_OBJECTS:${MODULE}_test_objs>)
  else()
    set(VANETZA_GTEST_OBJECTS "")
  endif()
endmacro()

# Add a test case using Google Testing Framework
## NAME name of the test case
## SRCS... variable number of source files
macro(add_gtest NAME)
  add_executable(GTest_${NAME} ${ARGN} ${VANETZA_GTEST_OBJECTS})
  set_target_properties(GTest_${NAME} PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
  target_link_libraries(GTest_${NAME} ${GTest_MAIN_LIBRARY})
  if(VANETZA_GTEST_LINK_LIBRARIES)
      target_link_libraries(GTest_${NAME} ${VANETZA_GTEST_LINK_LIBRARIES})
  endif()
  add_test(NAME ${NAME}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      COMMAND GTest_${NAME})
endmacro(add_gtest)

# Set target properties for a GTest
## NAME name of the test case
## ARGS passed to set_target_properties command
macro(set_gtest_properties NAME)
    set_target_properties(GTest_${NAME} PROPERTIES ${ARGN})
endmacro(set_gtest_properties)

# Link libraries to a GTest
## NAME name of the test case
## LIBS... variable number of libraries
macro(link_gtest NAME)
  target_link_libraries(GTest_${NAME} ${ARGN})
endmacro(link_gtest)

