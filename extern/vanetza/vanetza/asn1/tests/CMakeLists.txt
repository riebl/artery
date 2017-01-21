include(UseGTest)
configure_gtest_directory(LINK_LIBRARIES asn1)

add_gtest(asn1c_wrapper asn1c_wrapper.cpp)

