include(UseGTest)

add_gtest(Time time.cpp)
set_gtest_properties(Time INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR})
