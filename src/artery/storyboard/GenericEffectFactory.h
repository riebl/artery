#ifndef ARTERY_GNERICEFFECTFACTORY_H_ZBQPVHUE
#define ARTERY_GNERICEFFECTFACTORY_H_ZBQPVHUE

#include "artery/storyboard/EffectFactory.h"
#include <functional>

namespace artery
{

class STORYBOARD_API GenericEffectFactory : public EffectFactory
{
public:
    using FactoryFunction = std::function<std::shared_ptr<Effect>(Vehicle&, Story&, ConditionResult&)>;

    GenericEffectFactory(FactoryFunction fn);

    std::shared_ptr<Effect> create(Vehicle& v, Story& s, ConditionResult& r) override;

private:
    FactoryFunction mFactoryFunction;
};

} // namespace artery

#endif /* ARTERY_GNERICEFFECTFACTORY_H_ZBQPVHUE */

