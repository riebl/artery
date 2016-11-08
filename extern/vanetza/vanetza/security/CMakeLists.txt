find_package(Boost COMPONENTS serialization REQUIRED)
find_package(CryptoPP 5.6.1 REQUIRED)
find_package(OpenSSL 1.0.2)

set(CXX_SOURCES
    backend.cpp
    backend_cryptopp.cpp
    backend_null.cpp
    basic_elements.cpp
    certificate.cpp
    certificate_manager.cpp
    ecc_point.cpp
    ecdsa256.cpp
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

set(VANETZA_BACKEND_INCLUDE_DIRS ${CryptoPP_INCLUDE_DIR})
set(VANETZA_BACKEND_LIBRARIES ${CryptoPP_LIBRARY})
set(VANETZA_BACKEND_COMPILE_DEFINITIONS "VANETZA_WITH_CRYPTOPP")

if(OPENSSL_FOUND)
    list(APPEND CXX_SOURCES backend_openssl.cpp openssl_wrapper.cpp)
    list(APPEND VANETZA_BACKEND_INCLUDE_DIRS ${OPENSSL_INCLUDE_DIR})
    list(APPEND VANETZA_BACKEND_LIBRARIES ${OPENSSL_LIBRARIES})
    list(APPEND VANETZA_BACKEND_COMPILE_DEFINITIONS "VANETZA_WITH_OPENSSL")
endif()

vanetza_module(security ${CXX_SOURCES})
vanetza_intermodule_dependencies(security common net)

vanetza_module_property(security INTERFACE_INCLUDE_DIRECTORIES
    ${CryptoPP_INCLUDE_DIR} # because of NaivCertificateManager
)
vanetza_module_property(security INCLUDE_DIRECTORIES
    ${Boost_INCLUDE_DIRS}
    ${VANETZA_BACKEND_INCLUDE_DIRS}
)

vanetza_module_property(security COMPILE_DEFINITIONS ${VANETZA_BACKEND_COMPILE_DEFINITIONS})

vanetza_module_link_libraries(security
    ${Boost_SERIALIZATION_LIBRARY}
    ${VANETZA_BACKEND_LIBRARIES}
)

add_test_subdirectory(tests)
