/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ENVMOD_SENSORPOSITION_H_KCWHOGMB
#define ENVMOD_SENSORPOSITION_H_KCWHOGMB

#include <boost/units/quantity.hpp>
#include <boost/units/systems/angle/degrees.hpp>

namespace artery
{

enum class SensorPosition
{
    VIRTUAL,
    FRONT,
    BACK,
    LEFT,
    RIGHT
};

boost::units::quantity<boost::units::degree::plane_angle> relativeAngle(SensorPosition pos);

} // namespace artery

#endif /* ENVMOD_SENSORPOSITION_H_KCWHOGMB */
