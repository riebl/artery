vanetza_module(gnss nmea.cpp)
vanetza_module_property(gnss COMPILE_DEFINITIONS BOOST_DATE_TIME_NO_LIB)

add_test_subdirectory(tests)

