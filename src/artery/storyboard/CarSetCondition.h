#ifndef CARCONDITION_H
#define CARCONDITION_H

#include "artery/storyboard/Condition.h"
#include <string>
#include <set>

/**
 * Condition: CarSet
 * allows to specify the affected cars of a Story
 * each Story must only contain one CarCondition. If more cars are affected, add all cars to this condition
 */
class CarSetCondition : public Condition {
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
    bool testCondition(Veins::TraCIMobility* car);

private:
    std::set<std::string> m_cars;
};

#endif  /* CARCONDITION_H */
