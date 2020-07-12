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
add_subdirectory(${PROJECT_SOURCE_DIR}/extern/pybind11 pybind11)
target_link_libraries(storyboard PRIVATE pybind11::embed)
mark_as_advanced(PYBIND11_INSTALL PYBIND11_PYTHON_VERSION PYBIND11_TEST USE_PYTHON_INCLUDE_DIR)
