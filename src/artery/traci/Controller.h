#ifndef CONTROLLER_H_AXBS5NQM
#define CONTROLLER_H_AXBS5NQM

#include "artery/utility/Geometry.h"
#include "traci/API.h"
#include "traci/VariableCache.h"
#include <vanetza/units/acceleration.hpp>
#include <vanetza/units/angle.hpp>
#include <vanetza/units/length.hpp>
#include <vanetza/units/velocity.hpp>
#include <string>

namespace traci
{

class VariableCache;

class Controller
{
public:
    using Acceleration = vanetza::units::Acceleration;
    using Length = vanetza::units::Length;
    using Velocity = vanetza::units::Velocity;

    const std::string& getId() const;
    const std::shared_ptr<VariableCache>& getCache() const;
    const traci::Boundary& getBoundary() const;
    std::string getTypeId() const;

    artery::Position getPosition() const;
    artery::GeoPosition getGeoPosition() const;
    artery::Angle getHeading() const;
    Velocity getSpeed() const;

    Length getLength() const;
    Length getWidth() const;

    std::shared_ptr<traci::API> getTraCI() { return m_traci; }
    std::shared_ptr<const traci::API> getTraCI() const { return m_traci; }

    virtual ~Controller() {}

protected:
    Controller(std::shared_ptr<traci::API>, std::shared_ptr<VariableCache> cache);

    std::shared_ptr<traci::API> m_traci;
    traci::Boundary m_boundary;
    std::shared_ptr<VariableCache> m_cache;
};

} // namespace traci

#endif /* CONTROLLER_H_AXBS5NQM */

