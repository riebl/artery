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

target_link_libraries(storyboard PRIVATE pybind11::embed)
set_property(TARGET storyboard PROPERTY CXX_VISIBILITY_PRESET hidden)
