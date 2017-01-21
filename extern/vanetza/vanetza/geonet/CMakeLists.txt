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

add_vanetza_component(geonet ${CXX_SOURCES})
target_link_libraries(geonet PRIVATE GeographicLib::GeographicLib)
target_link_libraries(geonet PUBLIC Boost::date_time)
target_link_libraries(geonet PUBLIC dcc net security)

add_test_subdirectory(tests)
