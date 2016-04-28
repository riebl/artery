find_package(Boost COMPONENTS serialization REQUIRED)
find_package(CryptoPP 5.6.1 REQUIRED)

set(CXX_SOURCES
    basic_elements.cpp
    certificate.cpp
    certificate_manager.cpp
    ecc_point.cpp
    encryption_parameter.cpp
    header_field.cpp
    payload.cpp
    public_key.cpp
    length_coding.cpp
    int_x.cpp
    secured_message.cpp
    security_entity.cpp
    serialization.cpp
    signature.cpp
    signer_info.cpp
    subject_attribute.cpp
    subject_info.cpp
    recipient_info.cpp
    region.cpp
    trailer_field.cpp
    validity_restriction.cpp
)

vanetza_module(security ${CXX_SOURCES})
vanetza_intermodule_dependencies(security common net)

vanetza_module_property(security INCLUDE_DIRECTORIES
    ${Boost_INCLUDE_DIRS}
    ${CryptoPP_INCLUDE_DIR}
)

vanetza_module_link_libraries(security
    ${Boost_SERIALIZATION_LIBRARY}
    ${CryptoPP_LIBRARY}
)

add_test_subdirectory(tests)
