#ifndef ARTERY_CARCONDITION_H
#define ARTERY_CARCONDITION_H

#include "artery/storyboard/Condition.h"
#include <string>
#include <set>

namespace artery
{

/**
 * Condition: CarSet
 * allows to specify the affected cars of a Story
 */
class STORYBOARD_API CarSetCondition : public Condition {
public:

    /**
     * Creates a CarSetCondition and adds a set of cars to the list of affected cars
     */
    CarSetCondition(const std::set<std::string>&);

    /**
     * Creates a CarSetCondition and adds the given name to the list of affected cars
     */
    CarSetCondition(const std::string&);

    CarSetCondition() = default;

    /**
     * Tests if a car is specified in the car list
     * \param car to test
     * \return result of the test
     */
    ConditionResult testCondition(const Vehicle& car);

private:
    std::set<std::string> m_cars;
};

} // namespace artery

#endif  /* ARTERY_CARCONDITION_H */
