include(UseGTest)
configure_gtest_directory(LINK_LIBRARIES btp)

add_gtest(BtpDataIndication data_indication.cpp)
add_gtest(BtpHeader header.cpp)
add_gtest(BtpPortDispatcher port_dispatcher.cpp)
