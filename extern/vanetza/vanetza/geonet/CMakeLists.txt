find_package(Boost 1.56 COMPONENTS date_time REQUIRED)
find_package(GeographicLib 1.37 REQUIRED)

set(CXX_SOURCES
    address.cpp
    areas.cpp
    basic_header.cpp
    beacon_header.cpp
    cbf_packet_buffer.cpp
    common_header.cpp
    data_confirm.cpp
    data_indication.cpp
    data_request.cpp
    gbc_gac_header.cpp
    gbc_header.cpp
    header_type.cpp
    header_variant.cpp
    indication_context.cpp
    mib.cpp
    sequence_number.cpp
    lifetime.cpp
    location_table.cpp
    next_hop.cpp
    packet_buffer.cpp
    pdu.cpp
    pdu_conversion.cpp
    position_vector.cpp
    repeater.cpp
    router.cpp
    secured_pdu.cpp
    shb_header.cpp
    timestamp.cpp
    traffic_class.cpp
    variant_pdu.cpp
)

vanetza_module(geonet ${CXX_SOURCES})
vanetza_intermodule_dependencies(geonet dcc net security)

vanetza_module_property(geonet INCLUDE_DIRECTORIES
    ${Boost_INCLUDE_DIRS}
    ${GeographicLib_INCLUDE_DIR}
)
vanetza_module_link_libraries(geonet
    ${Boost_DATE_TIME_LIBRARY}
    ${GeographicLib_LIBRARY}
)

add_test_subdirectory(tests)
