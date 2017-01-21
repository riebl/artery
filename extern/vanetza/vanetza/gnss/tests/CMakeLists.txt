include(UseGTest)
configure_gtest_directory(LINK_LIBRARIES gnss)

add_gtest(NMEA nmea.cpp)

