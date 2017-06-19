cmake_minimum_required(VERSION 3.1)
project(Vanetza VERSION 0.8)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# Vanetza makes extensive use of C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build configuration
option(BUILD_USING_CONAN "Build with conan package manager" OFF)
if(BUILD_USING_CONAN)
    include(${PROJECT_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
endif()

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

option(BUILD_TESTS "Build unit tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(gtest)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib/static)

include(GNUInstallDirs)
include(UseVanetza)

# Enable usage of targets' folder property (good for Visual Studio)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Mandatory and optional project dependencies
set(Boost_COMPONENTS date_time serialization)
set(Boost_OPTIONAL_COMPONENTS system)
find_package(Boost 1.58 REQUIRED
  COMPONENTS ${Boost_COMPONENTS}
  OPTIONAL_COMPONENTS ${Boost_OPTIONAL_COMPONENTS})
include(CompatBoostTargets)
find_package(CryptoPP 5.6.1 REQUIRED)
find_package(GeographicLib 1.37 REQUIRED)

option(VANETZA_WITH_OPENSSL "Enable OpenSSL extensions" OFF)
if (VANETZA_WITH_OPENSSL)
  find_package(OpenSSL 1.0.1 REQUIRED)
  include(CompatOpenSSLTargets)
endif()

add_subdirectory(vanetza/asn1)
add_subdirectory(vanetza/btp)
add_subdirectory(vanetza/common)
add_subdirectory(vanetza/dcc)
add_subdirectory(vanetza/facilities)
add_subdirectory(vanetza/geonet)
add_subdirectory(vanetza/gnss)
add_subdirectory(vanetza/net)
add_subdirectory(vanetza/security)
add_subdirectory(vanetza/units)
add_subdirectory(tools/cohda_proxy)
add_subdirectory(tools/proxy_fake_feed)

option(BUILD_SOCKTAP "Build socktap application" OFF)
if(BUILD_SOCKTAP)
  add_subdirectory(tools/socktap)
endif()

# interface library for convenience
get_property(_components GLOBAL PROPERTY VANETZA_COMPONENTS)
add_library(vanetza INTERFACE)
foreach(_component IN LISTS _components)
  target_link_libraries(vanetza INTERFACE ${_component})
endforeach()
install(TARGETS vanetza EXPORT ${PROJECT_NAME})

# installation rules
include(CMakePackageConfigHelpers)
set(CMAKECONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/Vanetza")
set(CMAKECONFIG_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/cmake-config")
write_basic_package_version_file(${CMAKECONFIG_BUILD_DIR}/VanetzaConfigVersion.cmake
  COMPATIBILITY ExactVersion)
configure_package_config_file(cmake/VanetzaConfig.cmake.in
  ${CMAKECONFIG_BUILD_DIR}/VanetzaConfig.cmake
  INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
  PATH_VARS CMAKE_INSTALL_INCLUDEDIR)

install(FILES
  ${CMAKECONFIG_BUILD_DIR}/VanetzaConfig.cmake
  ${CMAKECONFIG_BUILD_DIR}/VanetzaConfigVersion.cmake
  DESTINATION ${CMAKECONFIG_INSTALL_DIR})

install(DIRECTORY ${PROJECT_SOURCE_DIR}/cmake/
  DESTINATION ${CMAKECONFIG_INSTALL_DIR}
  FILES_MATCHING
  PATTERN "MacroFindDependencyComponents.cmake"
  PATTERN "Find*.cmake"
  PATTERN "Compat*Targets.cmake")

install(EXPORT ${PROJECT_NAME} NAMESPACE Vanetza:: FILE VanetzaTargets.cmake DESTINATION ${CMAKECONFIG_INSTALL_DIR})

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/vanetza
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  FILES_MATCHING PATTERN "*.hpp" PATTERN "asn1/gen/*.h" PATTERN "/tests/*.hpp" EXCLUDE)

# export build tree (allows import by outside projects)
export(EXPORT ${PROJECT_NAME} NAMESPACE Vanetza:: FILE VanetzaExports.cmake)
file(COPY ${CMAKECONFIG_BUILD_DIR}/VanetzaConfigVersion.cmake
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
configure_file(cmake/VanetzaExportsConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/VanetzaConfig.cmake @ONLY)
export(PACKAGE ${PROJECT_NAME})

