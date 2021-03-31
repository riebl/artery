#ifndef VEHICLECONTROLLER_H_AXBS5NQM
#define VEHICLECONTROLLER_H_AXBS5NQM

#include "artery/traci/VehicleType.h"
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

class VehicleCache;

class VehicleController
{
public:
    using Acceleration = vanetza::units::Acceleration;
    using Length = vanetza::units::Length;
    using Velocity = vanetza::units::Velocity;

    VehicleController(std::shared_ptr<traci::API>, const std::string& id);
    VehicleController(std::shared_ptr<traci::API>, std::shared_ptr<VehicleCache> cache);

    const std::string& getVehicleId() const;
    std::string getTypeId() const;
    const VehicleType& getVehicleType() const;
    const std::string getVehicleClass() const;

    artery::Position getPosition() const;
    artery::GeoPosition getGeoPosition() const;
    artery::Angle getHeading() const;
    Velocity getSpeed() const;
    Velocity getMaxSpeed() const;
    void setMaxSpeed(Velocity);
    void setSpeed(Velocity);
    void setSpeedFactor(double);

    Length getLength() const;
    Length getWidth() const;

    void changeTarget(const std::string& edge);

    std::shared_ptr<traci::API> getTraCI() { return m_traci; }
    std::shared_ptr<const traci::API> getTraCI() const { return m_traci; }

private:
    std::shared_ptr<traci::API> m_traci;
    traci::Boundary m_boundary;
    VehicleType m_type;
    std::shared_ptr<VehicleCache> m_cache;
};

} // namespace traci

#endif /* VEHICLECONTROLLER_H_AXBS5NQM */

