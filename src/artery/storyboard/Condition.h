#ifndef _CONDITION_H_
#define _CONDITION_H_

#include "artery/storyboard/Vehicle.h"

/**
 * Condition Interface
 */
class Condition
{
public:
    virtual ~Condition() = default;
    virtual bool testCondition(const Vehicle& car) = 0;
};

#endif  /* CONDITION_H */
