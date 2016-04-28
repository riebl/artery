/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/envmod/sensor/SensorPosition.h"
#include <limits>

namespace artery
{

boost::units::quantity<boost::units::degree::plane_angle> relativeAngle(SensorPosition pos)
{
    using boost::units::degree::degree;
    using quantity = boost::units::quantity<boost::units::degree::plane_angle>;

    quantity angle;
    switch (pos) {
        case SensorPosition::FRONT:
            angle = 0.0 * degree;
            break;
        case SensorPosition::BACK:
            angle = 180.0 * degree;
            break;
        case SensorPosition::LEFT:
            angle = 90.0 * degree;
            break;
        case SensorPosition::RIGHT:
            angle = 270.0 * degree;
            break;
        default:
            angle = quantity::from_value(std::numeric_limits<double>::quiet_NaN());
    }

    return angle;
}

} // namespace artery
