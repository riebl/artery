include(UseGTest)
configure_gtest_directory(LINK_LIBRARIES facilities)

add_gtest(CamFunctions cam_functions.cpp)
add_gtest(PathHistory path_history.cpp)
add_gtest(PathPoint path_point.cpp)
