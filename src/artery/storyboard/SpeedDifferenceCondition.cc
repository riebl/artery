#include "artery/storyboard/SpeedDifferenceCondition.h"

namespace artery
{

ConditionResult SpeedDifferenceConditionFaster::testCondition(const Vehicle& car)
{
    std::set<const Vehicle*> affected;
    for (auto& other : car.getVehicles()) {
        if ((car.getController().getSpeed() - other.second.getController().getSpeed()) > mSpeedDifference) {
            affected.insert(&other.second);
        }
    }

    return affected;
}

ConditionResult SpeedDifferenceConditionSlower::testCondition(const Vehicle& car)
{
    std::set<const Vehicle*> affected;
    for (auto& other : car.getVehicles()) {
        if ((other.second.getController().getSpeed() - car.getController().getSpeed()) > mSpeedDifference) {
            affected.insert(&other.second);
        }
    }

    return affected;
}

} // namespace artery
