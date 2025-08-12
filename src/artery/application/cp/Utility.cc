/*
 * Artery V2X Simulation Framework
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/application/cp/Utility.h"


PerceivedObject* new_clone(const PerceivedObject& t)
{
    return static_cast<PerceivedObject*>(vanetza::asn1::copy(asn_DEF_PerceivedObject, &t));
}

void delete_clone(const PerceivedObject* t)
{
    vanetza::asn1::free(asn_DEF_PerceivedObject, const_cast<PerceivedObject*>(t));
}


namespace artery
{
namespace cp
{

std::pair<ObjectClass__class_PR, long> getObjectClass(artery::VehicleDataProvider::StationType stationType)
{
    switch (stationType) {
        case artery::VehicleDataProvider::StationType::Moped:
            return std::make_pair(ObjectClass__class_PR_vehicle, VehicleSubclassType_moped);
        case artery::VehicleDataProvider::StationType::Motorcycle:
            return std::make_pair(ObjectClass__class_PR_vehicle, VehicleSubclassType_motorcycle);
        case artery::VehicleDataProvider::StationType::Passenger_Car:
            return std::make_pair(ObjectClass__class_PR_vehicle, VehicleSubclassType_passengerCar);
        case artery::VehicleDataProvider::StationType::Bus:
            return std::make_pair(ObjectClass__class_PR_vehicle, VehicleSubclassType_bus);
        case artery::VehicleDataProvider::StationType::Light_Truck:
            return std::make_pair(ObjectClass__class_PR_vehicle, VehicleSubclassType_lightTruck);
        case artery::VehicleDataProvider::StationType::Heavy_Truck:
            return std::make_pair(ObjectClass__class_PR_vehicle, VehicleSubclassType_heavyTruck);
        case artery::VehicleDataProvider::StationType::Trailer:
            return std::make_pair(ObjectClass__class_PR_vehicle, VehicleSubclassType_trailer);
        case artery::VehicleDataProvider::StationType::Special_Vehicle:
            return std::make_pair(ObjectClass__class_PR_vehicle, VehicleSubclassType_specialVehicles);
        case artery::VehicleDataProvider::StationType::Tram:
            return std::make_pair(ObjectClass__class_PR_vehicle, VehicleSubclassType_tram);
        case artery::VehicleDataProvider::StationType::Pedestrian:
            return std::make_pair(ObjectClass__class_PR_person, PersonSubclassType_pedestrian);
        case artery::VehicleDataProvider::StationType::Cyclist:
            return std::make_pair(ObjectClass__class_PR_person, PersonSubclassType_cyclist);
        case artery::VehicleDataProvider::StationType::RSU:
            return std::make_pair(ObjectClass__class_PR_other, OtherSublassType_roadSideUnit);
        default:
            return std::make_pair(ObjectClass__class_PR_other, OtherSublassType_unknown);
    }
}

}  // namespace cp
}  // namespace artery
