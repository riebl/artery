/*
 * Artery V2X Simulation Framework
 * Copyright 2020 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_VEHICLEKINEMATICS_H_VDOQF76E
#define ARTERY_VEHICLEKINEMATICS_H_VDOQF76E

#include "artery/utility/Geometry.h"
#include <vanetza/units/acceleration.hpp>
#include <vanetza/units/angle.hpp>
#include <vanetza/units/velocity.hpp>
#include <vanetza/units/angular_velocity.hpp>

// forward declaration
namespace traci { class Controller; }

namespace artery
{

/**
 * VehicleKinematics stores attributes describing a vehicle's kinematic
 */
struct VehicleKinematics
{
    VehicleKinematics();

    Position position;
    GeoPosition geo_position;
    vanetza::units::Velocity speed;
    vanetza::units::Acceleration acceleration;
    vanetza::units::Angle heading; // from north, clockwise
    vanetza::units::AngularVelocity yaw_rate;
};

VehicleKinematics getKinematics(const traci::Controller&);

} // namespace artery

#endif /* ARTERY_VEHICLEKINEMATICS_H_VDOQF76E */

