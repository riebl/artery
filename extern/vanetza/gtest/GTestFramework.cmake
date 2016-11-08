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
set(GTest_HEADERS
    gtest.h
    gtest-spi.h
    gtest-death-test.h
    gtest-test-part.h
    gtest-param-test.h
    gtest-typed-test.h
    gtest_prod.h
    gtest-message.h
    gtest-printers.h
    gtest_pred_impl.h
    internal/gtest-linked_ptr.h
    internal/gtest-string.h
    internal/gtest-type-util.h
    internal/gtest-port-arch.h
    internal/gtest-filepath.h
    internal/gtest-param-util.h
    internal/gtest-tuple.h
    internal/gtest-internal.h
    internal/gtest-death-test-internal.h
    internal/gtest-port.h
    internal/gtest-param-util-generated.h
    internal/custom/gtest-printers.h
    internal/custom/gtest-port.h
    internal/custom/gtest.h
)
foreach(_header IN LISTS GTest_HEADERS)
    list(APPEND GTest_HEADERS ${GTest_INCLUDE_DIR}/gtest/${_header})
    list(REMOVE_ITEM GTest_HEADERS ${_header})
endforeach()

file(DOWNLOAD
    https://github.com/google/googletest/archive/${GTest_ARCHIVE}
    ${GTest_DIR}/${GTest_ARCHIVE}
    EXPECTED_HASH SHA1=${GTest_ARCHIVE_SHA1}
)
add_custom_target(download_gtest DEPENDS ${GTest_DIR}/${GTest_ARCHIVE})

add_custom_command(
    OUTPUT ${GTest_LIBRARY_SOURCES} ${GTest_MAIN_LIBRARY_SOURCES} ${GTest_HEADERS}
    COMMAND ${CMAKE_COMMAND} -E tar xfz ${GTest_DIR}/${GTest_ARCHIVE}
    DEPENDS download_gtest
    COMMENT "Extracting ${GTest_ARCHIVE} in ${GTest_DIR}"
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
add_library(${GTest_LIBRARY} ${GTest_LIBRARY_SOURCES} ${GTest_HEADERS})
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

# force building both targets in sequence because of shared custom command output
add_dependencies(${GTest_MAIN_LIBRARY} ${GTest_LIBRARY})

if(PTHREAD_LIBRARY)
    target_link_libraries(${GTest_LIBRARY} LINK_PUBLIC ${PTHREAD_LIBRARY})
endif()

