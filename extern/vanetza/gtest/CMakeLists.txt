include(GoogleTestTargets)

# export some variables into parent scope
set(GTest_LIBRARY ${GTest_LIBRARY} PARENT_SCOPE)
set(GTest_MAIN_LIBRARY ${GTest_MAIN_LIBRARY} PARENT_SCOPE)
set(GTest_INCLUDE_DIR ${GTest_INCLUDE_DIR} PARENT_SCOPE)
