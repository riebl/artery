#ifndef ANGLE_H_RTCIRJNH
#define ANGLE_H_RTCIRJNH

#include "artery/utility/Geometry.h"

namespace traci
{

/**
 * TraCI angle
 * - measured in degree
 * - 0 is headed north
 * - clockwise orientation (i.e. 90 heads east)
 * - range from 0 to 360
 */
struct TraCIAngle
{
    explicit TraCIAngle(double d) : degree(d) {}
    double degree;
};

Angle angle_cast(TraCIAngle);
TraCIAngle angle_cast(Angle);

} // namespace traci

#endif /* ANGLE_H_RTCIRJNH */

