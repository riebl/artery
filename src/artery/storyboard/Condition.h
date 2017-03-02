#ifndef _CONDITION_H_
#define _CONDITION_H_

#include "artery/storyboard/Vehicle.h"

namespace omnetpp { class cCanvas; }

/**
 * Condition Interface
 */
class Condition
{
public:
    virtual ~Condition() = default;
    virtual bool testCondition(const Vehicle& car) = 0;
    virtual void drawCondition(omnetpp::cCanvas*) {};
};

#endif  /* CONDITION_H */
