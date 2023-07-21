#ifndef ARTERY_CONDITION_H_
#define ARTERY_CONDITION_H_

#include "artery/storyboard/ConditionResult.h"
#include "artery/storyboard/Macros.h"
#include "artery/storyboard/Vehicle.h"

namespace omnetpp { class cCanvas; }

namespace artery
{

/**
 * Condition Interface
 */
class STORYBOARD_API Condition
{
public:
    virtual ~Condition() = default;
    virtual ConditionResult testCondition(const Vehicle& car) = 0;
    virtual void drawCondition(omnetpp::cCanvas*) {};
};

} // namespace artery

#endif  /* ARTERY_CONDITION_H */
