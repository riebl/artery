add_vanetza_component(security
    backend.cpp
    backend_null.cpp
    basic_elements.cpp
    certificate.cpp
    certificate_manager.cpp
    ecc_point.cpp
    ecdsa256.cpp
    encryption_parameter.cpp
    header_field.cpp
    int_x.cpp
    length_coding.cpp
    naive_certificate_manager.cpp
    null_certificate_manager.cpp
    payload.cpp
    public_key.cpp
    recipient_info.cpp
    region.cpp
    secured_message.cpp
    security_entity.cpp
    serialization.cpp
    signature.cpp
    sign_service.cpp
    signer_info.cpp
    subject_attribute.cpp
    subject_info.cpp
    trailer_field.cpp
    validity_restriction.cpp
    verify_service.cpp
)
target_link_libraries(security PUBLIC Boost::serialization common net)

# crypto++ is a public mandatory dependency because of "NaiveCertificateManager"
if(TARGET CryptoPP::CryptoPP)
    set_property(TARGET security APPEND PROPERTY
        SOURCES backend_cryptopp.cpp)
    target_link_libraries(security PUBLIC CryptoPP::CryptoPP)
    target_compile_definitions(security PUBLIC "VANETZA_WITH_CRYPTOPP")
endif()

if(TARGET OpenSSL::Crypto)
    set_property(TARGET security APPEND PROPERTY
        SOURCES backend_openssl.cpp openssl_wrapper.cpp)
    target_link_libraries(security PRIVATE OpenSSL::Crypto)
    target_compile_definitions(security PUBLIC "VANETZA_WITH_OPENSSL")
    if (${OPENSSL_VERSION} VERSION_LESS 1.1.0)
        target_compile_definitions(security PRIVATE "OPENSSL_API_COMPAT=0x10000000L")
    else()
        target_compile_definitions(security PRIVATE "OPENSSL_API_COMPAT=0x10100000L")
    endif()
endif()

add_test_subdirectory(tests)
