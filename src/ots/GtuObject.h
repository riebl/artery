#ifndef OTS_GTUOBJECT_H_Q7PI0NHO
#define OTS_GTUOBJECT_H_Q7PI0NHO

#include <omnetpp/cobject.h>
#include <tuple>

namespace ots
{

class GtuObject : public omnetpp::cObject
{
public:
    using Position = std::tuple<double, double, double>;
    GtuObject();

    void setId(const std::string& id) { m_id = id; }
    const std::string& getId() const { return m_id; }

    void setType(const std::string& type) { m_type = type; }
    const std::string& getType() const { return m_type; }

    void setPosition(const Position& pos) { m_position = pos; }
    const Position& getPosition() const { return m_position; }

    void setHeadingRad(double rad);
    void setHeadingDeg(double rad);
    double getHeadingRad() const;
    double getHeadingDeg() const;

    void setSpeed(double v) { m_speed = v; }
    double getSpeed() const { return m_speed; }

    void setAcceleration(double a) { m_acceleration = a; }
    double getAcceleration() const { return m_acceleration; }

private:
    std::string m_id;
    std::string m_type;
    Position m_position;
    double m_heading;
    double m_speed;
    double m_acceleration;
};

} // namespace ots

#endif /* OTS_GTUOBJECT_H_Q7PI0NHO */

