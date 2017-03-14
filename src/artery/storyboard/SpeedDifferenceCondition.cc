#include "artery/storyboard/SpeedDifferenceCondition.h"

ConditionResult SpeedDifferenceConditionFaster::testCondition(const Vehicle& car)
{
    std::set<Vehicle*> affected;
    for (auto& other : car.vehicles) {
        if ((car.controller.getSpeed() - other.second.controller.getSpeed()) > mSpeedDifference) {
            affected.insert(&other.second);
        }
    }

    return affected;
}

ConditionResult SpeedDifferenceConditionSlower::testCondition(const Vehicle& car)
{
    std::set<Vehicle*> affected;
    for (auto& other : car.vehicles) {
        if ((other.second.controller.getSpeed() - car.controller.getSpeed()) > mSpeedDifference) {
            affected.insert(&other.second);
        }
    }

    return affected;
}
