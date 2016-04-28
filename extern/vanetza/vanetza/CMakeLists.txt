find_package(Boost 1.50 REQUIRED)
set(VANETZA_MODULE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR})

add_subdirectory(asn1)
add_subdirectory(btp)
add_subdirectory(common)
add_subdirectory(dcc)
add_subdirectory(facilities)
add_subdirectory(geonet)
add_subdirectory(gnss)
add_subdirectory(net)
add_subdirectory(security)
add_subdirectory(units)

