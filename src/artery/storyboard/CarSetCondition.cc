#include "artery/storyboard/CarSetCondition.h"
#include "veins/modules/mobility/traci/TraCIMobility.h"
#include <algorithm>

CarSetCondition::CarSetCondition(const std::string& carName)
{
    m_cars.insert(carName);
}

CarSetCondition::CarSetCondition(const std::set<std::string>& carNames) :
    m_cars(carNames)
{
}

bool CarSetCondition::testCondition(const Vehicle& car)
{
    auto result = std::find(m_cars.begin(), m_cars.end(), car.mobility.getExternalId());
    return (result != m_cars.end());
}
