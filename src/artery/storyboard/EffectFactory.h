#ifndef ARTERY_EFFECTFACTORY_H_
#define ARTERY_EFFECTFACTORY_H_

#include "artery/storyboard/Condition.h"
#include "artery/storyboard/Effect.h"
#include "artery/storyboard/Macros.h"
#include <memory>

namespace artery
{

/**
 * EffectFactory Interface
 */
class STORYBOARD_API EffectFactory
{
public:
    virtual ~EffectFactory() = default;

    virtual std::shared_ptr<Effect> create(Vehicle&, Story&, ConditionResult&) = 0;
};

} // namespace artery

#endif /* ARTERY_EFFECTFACTORY_H_ */
