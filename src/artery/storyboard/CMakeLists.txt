add_artery_feature(storyboard
    AndCondition.cc
    Binding.cc
    CarSetCondition.cc
    ConditionResult.cc
    DeferringCondition.cc
    Effect.cc
    EffectStack.cc
    EmergencyStopEffect.cc
    GenericEffectFactory.cc
    LikelihoodCondition.cc
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

add_library(pybind11 INTERFACE)
if(NOT (CMAKE_VERSION VERSION_LESS 3.12))
    find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
    target_include_directories(pybind11 INTERFACE
        ${PROJECT_SOURCE_DIR}/extern/pybind11/include
        ${Python3_INCLUDE_DIRS})
    target_link_libraries(pybind11 INTERFACE ${Python3_LIBRARIES})
else()
    find_package(PythonLibs REQUIRED)
    target_include_directories(pybind11 INTERFACE
        ${PROJECT_SOURCE_DIR}/extern/pybind11/include
        ${PYTHON_INCLUDE_DIRS})
    target_link_libraries(pybind11 INTERFACE ${PYTHON_LIBRARIES})
endif()

target_link_libraries(storyboard PRIVATE pybind11)
set_property(TARGET storyboard PROPERTY CXX_VISIBILITY_PRESET hidden)