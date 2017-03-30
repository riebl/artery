find_package(Boost COMPONENTS python REQUIRED)
find_package(PythonLibs REQUIRED)

add_artery_feature(storyboard
    AndCondition.cc
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
    StoryboardSignal.cc
    Story.cc
    TimeCondition.cc
    TtcCondition.cc
    Vehicle.cc
)

target_include_directories(storyboard PUBLIC ${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS})
target_link_libraries(storyboard PUBLIC ${Boost_PYTHON_LIBRARY} ${PYTHON_LIBRARIES})
