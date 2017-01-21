set(GTest_VERSION 1.8.0)
set(GTest_ARCHIVE_NAME release-${GTest_VERSION})
set(GTest_ARCHIVE ${GTest_ARCHIVE_NAME}.tar.gz)
set(GTest_ARCHIVE_SHA1 e7e646a6204638fe8e87e165292b8dd9cd4c36ed)

set(GTest_DIR ${PROJECT_SOURCE_DIR}/gtest)
set(GTest_ARCHIVE_DIR ${GTest_DIR}/googletest-${GTest_ARCHIVE_NAME}/googletest)
set(GTest_SOURCE_DIR ${GTest_ARCHIVE_DIR}/src)
set(GTest_INCLUDE_DIR ${GTest_ARCHIVE_DIR}/include)

set(GTest_LIBRARY gtest)
set(GTest_LIBRARY_SOURCES ${GTest_SOURCE_DIR}/gtest-all.cc)
set(GTest_MAIN_LIBRARY gtest_main)
set(GTest_MAIN_LIBRARY_SOURCES ${GTest_SOURCE_DIR}/gtest_main.cc)

file(DOWNLOAD
    https://github.com/google/googletest/archive/${GTest_ARCHIVE}
    ${GTest_DIR}/${GTest_ARCHIVE}
    EXPECTED_HASH SHA1=${GTest_ARCHIVE_SHA1})
add_custom_target(download_gtest DEPENDS ${GTest_DIR}/${GTest_ARCHIVE})

add_custom_command(
    OUTPUT ${GTest_LIBRARY_SOURCES} ${GTest_MAIN_LIBRARY_SOURCES}
    COMMAND ${CMAKE_COMMAND} -E tar xfz ${GTest_DIR}/${GTest_ARCHIVE}
    DEPENDS download_gtest
    COMMENT "Extracting ${GTest_ARCHIVE} in ${GTest_DIR}"
    WORKING_DIRECTORY ${GTest_DIR}
    VERBATIM)
add_custom_target(extract_gtest DEPENDS ${GTest_LIBRARY_SOURCES} ${GTest_MAIN_LIBRARY_SOURCES})

# GTest library for tests with own main function
add_library(${GTest_LIBRARY} ${GTest_LIBRARY_SOURCES})
target_include_directories(${GTest_LIBRARY} PRIVATE ${GTest_ARCHIVE_DIR})
target_include_directories(${GTest_LIBRARY} SYSTEM PUBLIC ${GTest_INCLUDE_DIR})
add_dependencies(${GTest_LIBRARY} extract_gtest)

# GTest library providing main function for tests
add_library(${GTest_MAIN_LIBRARY} ${GTest_MAIN_LIBRARY_SOURCES})
target_link_libraries(${GTest_MAIN_LIBRARY} PUBLIC ${GTest_LIBRARY})
add_dependencies(${GTest_MAIN_LIBRARY} extract_gtest)

# Look for threading library and configure GTest accordingly
set(CMAKE_THREAD_PREFER_PTHREAD True)
set(THREADS_PREFER_PTHREAD_FLAG True)
find_package(Threads QUIET)
if(CMAKE_USE_PTHREADS_INIT)
    message(STATUS "Build GTest with pthread support")
    target_compile_definitions(${GTest_LIBRARY} PUBLIC "GTEST_HAS_PTHREAD=1")
    target_link_libraries(${GTest_LIBRARY} PUBLIC Threads::Threads)
else()
    message(STATUS "Build GTest without threading")
    target_compile_definitions(${GTest_LIBRARY} PUBLIC "GTEST_HAS_PTHREAD=0")
endif()

set_target_properties(download_gtest extract_gtest ${GTest_LIBRARY} ${GTest_MAIN_LIBRARY}
    PROPERTIES FOLDER "GTest")
