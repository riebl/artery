#ifndef ARTERY_DEN_CAUSECODE_H_P09HYXBX
#define ARTERY_DEN_CAUSECODE_H_P09HYXBX

#include <vanetza/asn1/denm.hpp>

namespace artery
{
namespace den
{

enum class CauseCode
{
    TrafficCondition = 1,
    Accident = 2,
    Roadworks = 3,
    AdverseWeatherCondition_Adhesion = 6,
    AdverseWeatherCondition_ExtremeWeather = 17,
    AdverseWeatherCondition_Visibility = 18,
    AdverseWeatherCondition_Precipitation = 19,
    HazardousLocation_SurfaceCondition = 9,
    HazardousLocation_ObstacleOnTheroad = 10,
    HazardousLocation_AnimalOnTheRoad = 11,
    HazardousLocation_DangerousCurve = 96,
    HumanPresenceOnTheRoad = 12,
    HumanProblem = 93,
    RescueAndRecoveryWorkInProgress = 15,
    EmergencyVehicleApproaching = 95,
    DangerousEndOfQueue = 27,
    SlowVehicle = 26,
    StationaryVehicle = 94,
    VehicleBreakdown = 91,
    PostCrash = 92,
    WrongWayDriving = 14,
    SignalViolation = 98,
    CollisionRisk = 97,
    DangerousSituation = 99
};

constexpr CauseCode convert(const CauseCodeType_t& type)
{
    return static_cast<CauseCode>(type);
}

} // namespace den
} // namespace artery

#endif /* ARTERY_DEN_CAUSECODE_H_P09HYXBX */

