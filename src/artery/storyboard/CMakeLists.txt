add_artery_feature(storyboard
    AndCondition.cc
    Binding.cc
    CarSetCondition.cc
    ConditionResult.cc
    Effect.cc
    EffectStack.cc
    LimitCondition.cc
    OrCondition.cc
    PolygonCondition.cc
    SignalEffect.cc
    SignalEffectFactory.cc
    SpeedDifferenceCondition.cc
    SpeedEffect.cc
    SpeedEffectFactory.cc
    StopEffect.cc
    StopEffectFactory.cc
    Storyboard.cc
    Story.cc
    TimeCondition.cc
    TtcCondition.cc
    Vehicle.cc
)

find_package(PythonLibs REQUIRED)
add_library(pybind11 INTERFACE)
target_include_directories(pybind11 INTERFACE
    ${PROJECT_SOURCE_DIR}/extern/pybind11/include
    ${PYTHON_INCLUDE_DIRS})
target_link_libraries(pybind11 INTERFACE ${PYTHON_LIBRARIES})
target_link_libraries(storyboard PRIVATE pybind11)
