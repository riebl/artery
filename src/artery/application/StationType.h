#ifndef ARTERY_STATIONTYPE_H_ORMPZL5X
#define ARTERY_STATIONTYPE_H_ORMPZL5X

#include <omnetpp/cobject.h>
#include <vanetza/geonet/station_type.hpp>

namespace artery
{

class StationType : public omnetpp::cObject
{
public:
    StationType() : m_type(vanetza::geonet::StationType::Unknown) {}
    StationType(vanetza::geonet::StationType type) : m_type(type) {}

    void setStationType(vanetza::geonet::StationType type) { m_type = type; }
    vanetza::geonet::StationType getStationType() const { return m_type; }

    inline StationType& operator=(vanetza::geonet::StationType type)
    {
        setStationType(type);
        return *this;
    }

    inline operator vanetza::geonet::StationType() const
    {
        return getStationType();
    }

private:
    vanetza::geonet::StationType m_type;
};

} // namespace artery

#endif /* ARTERY_STATIONTYPEOBJECT_H_ORMPZL5X */

