/*
 * Artery V2X Simulation Framework
 * Copyright 2016-2017 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_DENMOBJECT_H_
#define ARTERY_DENMOBJECT_H_

#include <omnetpp/cobject.h>
#include <boost/optional.hpp>
#include <vanetza/asn1/denm.hpp>
#include <memory>

namespace artery
{
namespace denm
{

enum class CauseCode {
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

CauseCode convert(const CauseCodeType_t&);

} // namespace denm

class DenmObject : public omnetpp::cObject
{
    public:
        DenmObject(vanetza::asn1::Denm&&);
        DenmObject(const vanetza::asn1::Denm&);
        DenmObject(const std::shared_ptr<vanetza::asn1::Denm>&);
        boost::optional<denm::CauseCode> situation_cause_code() const;
        vanetza::asn1::Denm& asn1();
        const vanetza::asn1::Denm& asn1() const;

        std::shared_ptr<const vanetza::asn1::Denm> shared_ptr() const;

    private:
        std::shared_ptr<vanetza::asn1::Denm> m_denm_wrapper;
};

bool operator&(const DenmObject&, denm::CauseCode);

} // namespace artery

#endif /* ARTERY_DENMOBJECT_H_ */
