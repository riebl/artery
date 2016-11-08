include(UseGTest)

add_gtest(Time time.cpp)
link_gtest(Time common)
