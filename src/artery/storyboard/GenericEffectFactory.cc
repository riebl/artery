#include "artery/storyboard/GenericEffectFactory.h"

namespace artery
{

GenericEffectFactory::GenericEffectFactory(FactoryFunction fn) :
    mFactoryFunction(fn)
{
}

std::shared_ptr<Effect> GenericEffectFactory::create(Vehicle& v, Story& s, ConditionResult& r)
{
    return mFactoryFunction(v, s, r);
}

} // namespace artery
