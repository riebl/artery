#ifndef EFFECTFACTORY_H_
#define EFFECTFACTORY_H_

#include "artery/storyboard/Effect.h"
#include <memory>

/**
 * EffectFactory Interface
 */
class EffectFactory
{
public:
    virtual ~EffectFactory() = default;

    virtual std::shared_ptr<Effect> create(traci::VehicleController&, Story*) = 0;
};

#endif /* EFFECTFACTORY_H_ */
