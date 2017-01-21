include(CMakeParseArguments)

#
# configure_gtest_directory(
#   [SOURCES <srcs...>]
#   [LINK_LIBRARIES <libs...>]
#   [INCLUDE_DIRECTORIES <dirs...>]
#   [COMPILE_DEFINITIONS <defs...>])
#
# Set GTest properties common for all unit test executables in current directory.
#
function(configure_gtest_directory)
  cmake_parse_arguments(args "" "" "SOURCES;LINK_LIBRARIES;INCLUDE_DIRECTORIES;COMPILE_DEFINITIONS" ${ARGN})
  if(args_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "unknown arguments given to configure_gtest_directory: ${args_UNPARSED_ARGUMENTS}")
  endif()

  set_property(DIRECTORY PROPERTY GTEST_SOURCES ${args_SOURCES})
  set_property(DIRECTORY PROPERTY GTEST_LINK_LIBRARIES ${args_LINK_LIBRARIES})
  set_property(DIRECTORY PROPERTY GTEST_INCLUDE_DIRECTORIES ${args_INCLUDE_DIRECTORIES})
  set_property(DIRECTORY PROPERTY GTEST_COMPILE_DEFINITIONS ${args_COMPILE_DEFINITIONS})
endfunction()

#
# add_gtest(<name>
#   [SOURCES] <srcs...>
#   [LINK_LIBRARIES <libs...>]
#   [INCLUDE_DIRECTORIES <dirs...>]
#   [COMPILE_DEFINITIONS <defs...>])
#
# Add a CTest unit test executable using Google Test.
# The executable will be built as specified by given arguments and directory properties.
#
function(add_gtest name)
  set(target GTest_${name})
  cmake_parse_arguments(args "" "" "SOURCES;LINK_LIBRARIES;INCLUDE_DIRECTORIES;COMPILE_DEFINITIONS" ${ARGN})

  get_directory_property(sources GTEST_SOURCES)
  list(APPEND sources "${args_SOURCES}" "${args_UNPARSED_ARGUMENTS}")
  get_directory_property(link_libraries GTEST_LINK_LIBRARIES)
  list(APPEND link_libraries "${args_LINK_LIBRARIES}" ${GTest_MAIN_LIBRARY})
  get_directory_property(include_directories GTEST_INCLUDE_DIRECTORIES)
  list(APPEND include_directories "${args_INCLUDE_DIRECTORIES}" ${GTest_INCLUDE_DIR})
  get_directory_property(compile_definitions GTEST_COMPILE_DEFINITIONS)
  list(APPEND compile_definitions "${args_COMPILE_DEFINITIONS}")

  if(NOT sources)
    message(FATAL_ERROR "no sources given to add_gtest")
  endif()

  add_executable(${target} ${sources})
  set_target_properties(${target} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests
    FOLDER "GTest"
    LINK_LIBRARIES "${link_libraries}"
    INCLUDE_DIRECTORIES "${include_directories}"
    COMPILE_DEFINITIONS "${compile_definitions}")
  add_test(NAME ${name}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND ${target})
endfunction()
