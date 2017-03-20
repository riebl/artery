#ifndef EFFECTFACTORY_H_
#define EFFECTFACTORY_H_

#include "artery/storyboard/Condition.h"
#include "artery/storyboard/Effect.h"
#include <memory>

/**
 * EffectFactory Interface
 */
class EffectFactory
{
public:
    virtual ~EffectFactory() = default;

    virtual std::shared_ptr<Effect> create(Vehicle&, Story&, ConditionResult&) = 0;
};

#endif /* EFFECTFACTORY_H_ */
