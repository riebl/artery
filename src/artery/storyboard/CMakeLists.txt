find_package(Boost COMPONENTS python REQUIRED)
find_package(PythonLibs REQUIRED)

set(STORYBOARD_SOURCES
    AndCondition.cc
    CarSetCondition.cc
    Effect.cc
    EffectStack.cc
    OrCondition.cc
    PolygonCondition.cc
    SpeedEffect.cc
    SpeedEffectFactory.cc
    Storyboard.cc
    Story.cc
    TimeCondition.cc
)

add_library(storyboard SHARED ${STORYBOARD_SOURCES})
target_link_libraries(storyboard PUBLIC artery)
target_link_libraries(storyboard PUBLIC ${Boost_PYTHON_LIBRARY} ${PYTHON_LIBRARIES})
target_include_directories(storyboard PUBLIC ${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS})
