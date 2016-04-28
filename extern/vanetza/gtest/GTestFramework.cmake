set(GTest_VERSION d945d8c000a0ade73585d143532266968339bbb3)
set(GTest_ARCHIVE ${GTest_VERSION}.zip)
set(GTest_ARCHIVE_SHA1 e60441b3b02f0de37a7b1afc3e83ba83aac00f52)

set(GTest_DIR ${PROJECT_SOURCE_DIR}/gtest)
set(GTest_ARCHIVE_DIR ${GTest_DIR}/googletest-${GTest_VERSION}/googletest)
set(GTest_SOURCE_DIR ${GTest_ARCHIVE_DIR}/src)
set(GTest_INCLUDE_DIR ${GTest_ARCHIVE_DIR}/include)

set(GTest_LIBRARY gtest)
set(GTest_LIBRARY_SOURCES ${GTest_SOURCE_DIR}/gtest-all.cc)
set(GTest_MAIN_LIBRARY gtest_main)
set(GTest_MAIN_LIBRARY_SOURCES ${GTest_SOURCE_DIR}/gtest_main.cc)

file(DOWNLOAD
    https://github.com/google/googletest/archive/${GTest_ARCHIVE}
    ${GTest_DIR}/${GTest_ARCHIVE}
    EXPECTED_HASH SHA1=${GTest_ARCHIVE_SHA1}
)

add_custom_command(
    OUTPUT ${GTest_LIBRARY_SOURCES} ${GTest_MAIN_LIBRARY_SOURCES}
    COMMAND ${CMAKE_COMMAND} -E tar x ${GTest_ARCHIVE}
    DEPENDS ${GTest_DIR}/${GTest_ARCHIVE}
    WORKING_DIRECTORY ${GTest_DIR}
    VERBATIM
)

find_library(PTHREAD_LIBRARY NAMES pthread)
if(PTHREAD_LIBRARY)
    set(GTest_DEFINITIONS "GTEST_HAS_PTHREAD=1")
else()
    set(GTest_DEFINITIONS "GTEST_HAS_PTHREAD=0")
endif()

# GTest library for tests with own main function
add_library(${GTest_LIBRARY} ${GTest_LIBRARY_SOURCES})
target_include_directories(${GTest_LIBRARY}
    PRIVATE ${GTest_ARCHIVE_DIR})
target_include_directories(${GTest_LIBRARY} SYSTEM
    PUBLIC ${GTest_INCLUDE_DIR})
target_compile_definitions(${GTest_LIBRARY}
    PUBLIC ${GTest_DEFINITIONS})

# GTest library providing main function for tests
add_library(${GTest_MAIN_LIBRARY} ${GTest_MAIN_LIBRARY_SOURCES})
target_include_directories(${GTest_MAIN_LIBRARY} SYSTEM
    PRIVATE ${GTest_INCLUDE_DIR})
target_compile_definitions(${GTest_MAIN_LIBRARY}
    PRIVATE ${GTest_DEFINITIONS})
target_link_libraries(${GTest_MAIN_LIBRARY} LINK_INTERFACE_LIBRARIES ${GTest_LIBRARY})

if(PTHREAD_LIBRARY)
    target_link_libraries(${GTest_LIBRARY} LINK_PUBLIC ${PTHREAD_LIBRARY})
endif()

