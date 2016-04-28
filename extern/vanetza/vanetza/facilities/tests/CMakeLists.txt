include(UseGTest)
prepare_gtest(facilities)

add_gtest(PathHistory path_history.cpp)
add_gtest(PathPoint path_point.cpp)
