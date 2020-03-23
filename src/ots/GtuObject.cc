#include <ots/GtuObject.h>
#include <omnetpp.h>
#include <cmath>
#include <limits>

namespace ots
{

Register_Abstract_Class(GtuObject)

namespace
{

const double nan = std::numeric_limits<double>::quiet_NaN();
const double pi = std::acos(-1.0);

} // namespace

GtuObject::GtuObject() :
    m_position(nan, nan, nan), m_heading(nan), m_acceleration(nan)
{
}

void GtuObject::setHeadingRad(double rad)
{
    m_heading = std::fmod(rad, 2.0 * pi);
    if (m_heading < 0.0) {
        m_heading += 2.0 * pi;
    }
}

void GtuObject::setHeadingDeg(double deg)
{
    setHeadingRad(deg / 180.0 * pi);
}

double GtuObject::getHeadingRad() const
{
    return m_heading;
}

double GtuObject::getHeadingDeg() const
{
    return m_heading / pi * 180.0;
}

} // namespace ots
