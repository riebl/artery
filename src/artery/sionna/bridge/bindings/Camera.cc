#include "Camera.h"

#include <artery/sionna/bridge/Helpers.h>

using namespace artery::sionna;
using namespace artery::sionna::literals;

const char* py::Camera::className() const
{
    return "Camera";
}

py::Camera::Camera(nanobind::object obj)
{
    WrapPythonClassCapability::init(std::move(obj));
}

py::Camera::Camera(const mi::Point3f& position, const mi::Point3f& orientation)
{
    InitPythonClassCapability::init("position"_a = position, "orientation"_a = orientation);
}

mi::Point3f py::Camera::position() const
{
    return sionna::access<mi::Point3f>(bound_, "position");
}

void py::Camera::setPosition(const mi::Point3f& position)
{
    sionna::set(bound_, "position", position);
}

mi::Point3f py::Camera::orientation() const
{
    return sionna::access<mi::Point3f>(bound_, "orientation");
}

void py::Camera::setOrientation(const mi::Point3f& orientation)
{
    sionna::set(bound_, "orientation", orientation);
}
