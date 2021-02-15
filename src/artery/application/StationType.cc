#include "artery/application/StationType.h"

namespace artery
{

StationType& StationType::operator=(vanetza::geonet::StationType type)
{
    setStationType(type);
    return *this;
}

StationType::operator vanetza::geonet::StationType() const
{
    return getStationType();
}

vanetza::geonet::StationType deriveStationTypeFromVehicleClass(const std::string& vclass)
{
    using vanetza::geonet::StationType;
    StationType gnStationType;
    if (vclass == "passenger" || vclass == "private" || vclass == "taxi") {
        gnStationType = StationType::Passenger_Car;
    } else if (vclass == "coach" || vclass == "delivery") {
        gnStationType = StationType::Light_Truck;
    } else if (vclass == "truck") {
        gnStationType = StationType::Heavy_Truck;
    } else if (vclass == "trailer") {
        gnStationType = StationType::Trailer;
    } else if (vclass == "bus") {
        gnStationType = StationType::Bus;
    } else if (vclass == "emergency" || vclass == "authority") {
        gnStationType = StationType::Special_Vehicle;
    } else if (vclass == "moped") {
        gnStationType = StationType::Moped;
    } else if (vclass == "motorcycle") {
        gnStationType = StationType::Motorcycle;
    } else if (vclass == "tram") {
        gnStationType = StationType::Tram;
    } else if (vclass == "bicycle") {
        gnStationType = StationType::Cyclist;
    } else if (vclass == "pedestrian") {
        gnStationType = StationType::Pedestrian;
    } else {
        gnStationType = StationType::Unknown;
    }

    return gnStationType;
}

} // namespace artery
