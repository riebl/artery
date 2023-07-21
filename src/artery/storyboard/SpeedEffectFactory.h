#ifndef ARTERY_SPEEDEFFECTFACTORY_H_
#define ARTERY_SPEEDEFFECTFACTORY_H_

#include "artery/storyboard/Effect.h"
#include "artery/storyboard/EffectFactory.h"
#include "artery/traci/VehicleController.h"
#include <memory>

namespace artery
{

/**
 * SpeedEffectFactory creates SpeedEffects
 */
class STORYBOARD_API SpeedEffectFactory : public EffectFactory
{
public:
    SpeedEffectFactory(double speed) :
        m_speed(speed)
    {
    }

    /**
     * Creates a new Effect for a TraCIMobility
     * \param Vehicle which should be affected from the Effect
     * \param Story from which the Effect should be created
     * \return Effect to add on the EffectStack from the TraCIMobility
     */
    std::shared_ptr<Effect> create(Vehicle&, Story&, ConditionResult&) override;

private:
    double m_speed;
};

} // namespace artery

#endif /* ARTERY_SPEEDEFECTFACTORY_H_ */
