#ifndef ARTERY_STATIONTYPE_H_ORMPZL5X
#define ARTERY_STATIONTYPE_H_ORMPZL5X

#include <omnetpp/cobject.h>
#include <vanetza/geonet/station_type.hpp>
#include <string>

namespace artery
{

class StationType : public omnetpp::cObject
{
public:
    StationType() : m_type(vanetza::geonet::StationType::Unknown) {}
    StationType(vanetza::geonet::StationType type) : m_type(type) {}

    // explicit getter and setter
    void setStationType(vanetza::geonet::StationType type) { m_type = type; }
    vanetza::geonet::StationType getStationType() const { return m_type; }

    // implicit conversion from and to vanetza::geonet::StationType
    StationType& operator=(vanetza::geonet::StationType type);
    operator vanetza::geonet::StationType() const;

private:
    vanetza::geonet::StationType m_type;
};

/**
 * Matches a SUMO vehicle class to an ITS station type.
 * \see https://sumo.dlr.de/docs/Definition_of_Vehicles,_Vehicle_Types,_and_Routes.html#abstract_vehicle_class
 * for the vehicle classes defined by SUMO.
 *
 * \param SUMO vehicle class
 * \return matching ITS station type, falls back to "UNKNOWN"
 */
vanetza::geonet::StationType deriveStationTypeFromVehicleClass(const std::string& vclass);

} // namespace artery

#endif /* ARTERY_STATIONTYPEOBJECT_H_ORMPZL5X */

