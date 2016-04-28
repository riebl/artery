include(UseGTest)

prepare_gtest(asn1)
add_gtest(asn1c_wrapper asn1c_wrapper.cpp)

