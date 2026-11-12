#pragma once

#include <artery/sionna/bridge/SionnaBridge.h>
#include <artery/traci/Cast.h>
#include <inet/common/geometry/common/Coord.h>
#include <inet/common/geometry/common/EulerAngles.h>
#include <mitsuba/core/fwd.h>
#include <traci/Angle.h>
#include <traci/Position.h>

#include <numbers>

namespace artery::sionna
{

struct TraCIVelocity {
    double speed;
    traci::TraCIAngle heading;
};

template <typename Value>
struct impl<inet::Coord, mitsuba::Point<Value, 3>, void> {
    static inet::Coord convert(const mitsuba::Point<Value, 3>& value) { return inet::Coord(toScalar(value[0]), toScalar(value[1]), toScalar(value[2])); }
};

template <typename Value>
struct impl<inet::Coord, mitsuba::Vector<Value, 3>, void> {
    static inet::Coord convert(const mitsuba::Vector<Value, 3>& value) { return inet::Coord(toScalar(value[0]), toScalar(value[1]), toScalar(value[2])); }
};

template <typename Value>
struct impl<inet::Coord, mitsuba::Normal<Value, 3>, void> {
    static inet::Coord convert(const mitsuba::Normal<Value, 3>& value) { return inet::Coord(toScalar(value[0]), toScalar(value[1]), toScalar(value[2])); }
};

template <typename Value>
struct impl<inet::Coord, mitsuba::BoundingBox<mitsuba::Point<Value, 3>>, void> {
    static inet::Coord convert(const mitsuba::BoundingBox<mitsuba::Point<Value, 3>>& value)
    {
        return artery::sionna::convert<inet::Coord>(value.max - value.min);
    }
};

template <typename Value>
struct impl<inet::EulerAngles, mitsuba::Point<Value, 3>, void> {
    static inet::EulerAngles convert(const mitsuba::Point<Value, 3>& value)
    {
        const inet::Coord coord = artery::sionna::convert<inet::Coord>(value);
        return inet::EulerAngles(coord.x, coord.y, coord.z);
    }
};

template <>
struct impl<mitsuba::Resolve::Point3f, inet::Coord, void> {
    static mitsuba::Resolve::Point3f convert(const inet::Coord& value) { return mitsuba::Resolve::Point3f(value.x, value.y, value.z); }
};

template <>
struct impl<mitsuba::Resolve::Vector3f, inet::Coord, void> {
    static mitsuba::Resolve::Vector3f convert(const inet::Coord& value) { return mitsuba::Resolve::Vector3f(value.x, value.y, value.z); }
};

template <>
struct impl<mitsuba::Resolve::Point3f, inet::EulerAngles, void> {
    static mitsuba::Resolve::Point3f convert(const inet::EulerAngles& value) { return mitsuba::Resolve::Point3f(value.alpha, value.beta, value.gamma); }
};

template <>
struct impl<mitsuba::Resolve::Point3f, traci::TraCIPosition, void> {
    static mitsuba::Resolve::Point3f convert(const traci::TraCIPosition& value) { return mitsuba::Resolve::Point3f(value.x, value.y, value.z); }
};

template <>
struct impl<mitsuba::Resolve::Vector3f, traci::TraCIPosition, void> {
    static mitsuba::Resolve::Vector3f convert(const traci::TraCIPosition& value) { return mitsuba::Resolve::Vector3f(value.x, value.y, value.z); }
};

template <>
struct impl<mitsuba::Resolve::Point3f, artery::Position, void> {
    static mitsuba::Resolve::Point3f convert(const artery::Position& value) { return mitsuba::Resolve::Point3f(value.x.value(), value.y.value(), 0.0); }
};

template <>
struct impl<mitsuba::Resolve::Vector3f, artery::Position, void> {
    static mitsuba::Resolve::Vector3f convert(const artery::Position& value) { return mitsuba::Resolve::Vector3f(value.x.value(), value.y.value(), 0.0); }
};

template <typename Value>
struct impl<artery::Position, mitsuba::Point<Value, 3>, void> {
    static artery::Position convert(const mitsuba::Point<Value, 3>& value) { return artery::Position(toScalar(value[0]), toScalar(value[1])); }
};

template <typename Value>
struct impl<artery::Position, mitsuba::Vector<Value, 3>, void> {
    static artery::Position convert(const mitsuba::Vector<Value, 3>& value) { return artery::Position(toScalar(value[0]), toScalar(value[1])); }
};

template <typename Value>
struct impl<libsumo::TraCIPosition, mitsuba::Point<Value, 3>, void> {
    static libsumo::TraCIPosition convert(const mitsuba::Point<Value, 3>& value)
    {
        libsumo::TraCIPosition result;
        result.x = toScalar(value[0]);
        result.y = toScalar(value[1]);
        result.z = toScalar(value[2]);
        return result;
    }
};

template <typename Value>
struct impl<libsumo::TraCIPosition, mitsuba::Vector<Value, 3>, void> {
    static libsumo::TraCIPosition convert(const mitsuba::Vector<Value, 3>& value)
    {
        libsumo::TraCIPosition result;
        result.x = toScalar(value[0]);
        result.y = toScalar(value[1]);
        result.z = toScalar(value[2]);
        return result;
    }
};

template <>
struct impl<mitsuba::Resolve::Point3f, traci::TraCIAngle, void> {
    static mitsuba::Resolve::Point3f convert(const traci::TraCIAngle& value)
    {
        const double yaw = (90.0 - value.degree) * std::numbers::pi_v<double> / 180.0;
        return mitsuba::Resolve::Point3f(0.0, 0.0, yaw);
    }
};

template <>
struct impl<mitsuba::Resolve::Vector3f, TraCIVelocity, void> {
    static mitsuba::Resolve::Vector3f convert(const TraCIVelocity& value)
    {
        const double yaw = (90.0 - value.heading.degree) * std::numbers::pi_v<double> / 180.0;
        return mitsuba::Resolve::Vector3f(value.speed * std::cos(yaw), value.speed * -std::sin(yaw), 0.0);
    }
};

}  // namespace artery::sionna
