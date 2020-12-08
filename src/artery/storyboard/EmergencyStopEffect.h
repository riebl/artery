#ifndef ARTERY_EMERGENCYSTOPEFFECT_H_
#define ARTERY_EMERGENCYSTOPEFFECT_H_

#include "artery/storyboard/Effect.h"

namespace artery
{

/**
 * EmergencyStopEffect stops vehicle with emergency deceleration.
 * Once stopped vehicle remains stopped.
 */
class EmergencyStopEffect : public Effect
{
public:
    using Effect::Effect;

    void applyEffect() override;
    void reapplyEffect() override;
    void removeEffect() override;
};

} // namespace artery

#endif /* ARTERY_EMERGENCYSTOPEFFECT_H_ */
