include(UseGTest)
prepare_gtest(security
    check_basic_elements.cpp
    check_certificate.cpp
    check_encryption_parameter.cpp
    check_header_field.cpp
    check_public_key.cpp
    check_recipient_info.cpp
    check_region.cpp
    check_secured_message.cpp
    check_signature.cpp
    check_signer_info.cpp
    check_subject_attribute.cpp
    check_trailer_field.cpp
    check_validity_restriction.cpp
    serialization.cpp
)
target_link_libraries(security_test INTERFACE common geonet)

add_gtest(Certificate certificate.cpp)
add_gtest(CertificateManager certificate_manager.cpp)
add_gtest(EccPoint ecc_point.cpp)
add_gtest(EncryptionParameter encryption_parameter.cpp)
add_gtest(HeaderField header_field.cpp)
add_gtest(IntX int_x.cpp)
add_gtest(LengthEncoding length_encoding.cpp)
add_gtest(Payload payload.cpp)
add_gtest(PublicKey public_key.cpp)
add_gtest(RecipientInfo recipient_info.cpp)
add_gtest(Region region.cpp)
add_gtest(SecuredMessage secured_message.cpp)
add_gtest(Signature signature.cpp)
add_gtest(SignerInfo signer_info.cpp)
add_gtest(SubjectAttribute subject_attribute.cpp)
add_gtest(SubjectInfo subject_info.cpp)
add_gtest(TrailerField trailer_field.cpp)
add_gtest(ValidityRestriction validity_restriction.cpp)
add_gtest(SecurityEntity security_entity.cpp)
