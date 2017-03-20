find_package(Boost COMPONENTS python REQUIRED)
find_package(PythonLibs REQUIRED)

set(STORYBOARD_SOURCES
    AndCondition.cc
    CarSetCondition.cc
    Effect.cc
    EffectStack.cc
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

add_library(storyboard SHARED ${STORYBOARD_SOURCES})
target_include_directories(storyboard PUBLIC $<TARGET_PROPERTY:artery,INCLUDE_DIRECTORIES>)
target_include_directories(storyboard PUBLIC ${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS})
target_link_libraries(storyboard PUBLIC ${Boost_PYTHON_LIBRARY} ${PYTHON_LIBRARIES})
