#include "RadioDevice.h"

#include <artery/sionna/bridge/Helpers.h>

using namespace artery::sionna;
using namespace artery::sionna::literals;

py::RadioDevice::RadioDevice(nanobind::object obj)
{
    WrapPythonClassCapability::init(std::move(obj));
}

std::string py::RadioDevice::name() const
{
    return sionna::access<std::string>(bound_, "name");
}

mi::Point3f py::RadioDevice::position() const
{
    return sionna::access<mi::Point3f>(bound_, "position");
}

void py::RadioDevice::setPosition(const mi::Point3f& position)
{
    sionna::set(bound_, "position", position);
}

mi::Point3f py::RadioDevice::orientation() const
{
    return sionna::access<mi::Point3f>(bound_, "orientation");
}

void py::RadioDevice::setOrientation(const mi::Point3f& orientation)
{
    sionna::set(bound_, "orientation", orientation);
}

mi::Vector3f py::RadioDevice::velocity() const
{
    return sionna::access<mi::Vector3f>(bound_, "velocity");
}

void py::RadioDevice::setVelocity(const mi::Vector3f& velocity)
{
    sionna::set(bound_, "velocity", velocity);
}

auto py::RadioDevice::color() const -> TColor
{
    return sionna::access<TColor>(bound_, "color");
}

void py::RadioDevice::setColor(TColor color)
{
    sionna::set(bound_, "color", std::move(color));
}

void py::RadioDevice::lookAt(const mi::Point3f& target)
{
    sionna::call(bound_, "look_at", "target"_a = target);
}

const char* py::Transmitter::className() const
{
    return "Transmitter";
}

py::Transmitter::Transmitter(nanobind::object obj) : RadioDevice(std::move(obj))
{
}

py::Transmitter::Transmitter(const std::string& name, const mi::Point3f& position, std::optional<mi::Point3f> orientation, std::optional<mi::Vector3f> velocity)
{
    InitPythonClassCapability::init("name"_a = name, "position"_a = position, "orientation"_a = orientation, "velocity"_a = velocity);
}

maybe_diff_t<mi::Float> py::Transmitter::power() const
{
    return sionna::access<maybe_diff_t<mi::Float>>(bound_, "power");
}

maybe_diff_t<mi::Float> py::Transmitter::powerDbm() const
{
    return sionna::access<maybe_diff_t<mi::Float>>(bound_, "power_dbm");
}

void py::Transmitter::setPowerDbm(maybe_diff_t<mi::Float> powerDbm)
{
    sionna::set(bound_, "power_dbm", powerDbm);
}

const char* py::Receiver::className() const
{
    return "Receiver";
}

py::Receiver::Receiver(nanobind::object obj) : RadioDevice(std::move(obj))
{
}

py::Receiver::Receiver(const std::string& name, const mi::Point3f& position, std::optional<mi::Point3f> orientation, std::optional<mi::Vector3f> velocity)
{
    InitPythonClassCapability::init("name"_a = name, "position"_a = position, "orientation"_a = orientation, "velocity"_a = velocity);
}
