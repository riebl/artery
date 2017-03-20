#ifndef _CONDITION_H_
#define _CONDITION_H_

#include "artery/storyboard/Vehicle.h"
#include <boost/variant.hpp>
#include <set>

namespace omnetpp { class cCanvas; }

typedef boost::variant<bool, std::set<const Vehicle*>> ConditionResult;

/**
 * Condition Interface
 */
class Condition
{
public:
    virtual ~Condition() = default;
    virtual ConditionResult testCondition(const Vehicle& car) = 0;
    virtual void drawCondition(omnetpp::cCanvas*) {};
};

#endif  /* CONDITION_H */
