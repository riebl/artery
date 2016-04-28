cmake_minimum_required(VERSION 3.0)
cmake_policy(VERSION 3.0)
project(Vanetza)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib/static)
include(VanetzaModule)


if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

option(ENABLE_TESTS "Build unit tests" OFF)
if(ENABLE_TESTS)
    enable_testing()
    include(gtest/GTestFramework.cmake)
endif()

# Add subdirectory only when tests are enabled
## DIR subdirectory name
macro(add_test_subdirectory DIR)
  if(ENABLE_TESTS)
    add_subdirectory(${DIR})
  endif(ENABLE_TESTS)
endmacro(add_test_subdirectory)


add_subdirectory(vanetza)
add_subdirectory(tools/cohda_proxy)
add_subdirectory(tools/proxy_fake_feed)
vanetza_export_modules()

