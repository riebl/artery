// for ref.
// clang-format off
#include <nanobind/nanobind.h>
// clang-format on

#include "SceneObject.h"

#include <artery/sionna/bridge/Helpers.h>
#include <mitsuba/core/vector.h>
#include <mitsuba/render/mesh.h>

using namespace artery::sionna;
using namespace artery::sionna::literals;

py::SceneObject::SceneObject(nanobind::object obj)
{
    WrapPythonClassCapability::init(std::move(obj));
}

py::SceneObject::SceneObject(mitsuba::ref<mi::Mesh> mesh, const py::RadioMaterial& material)
{
    this->InitPythonClassCapability::init("mi_mesh"_a = mesh.get(), "radio_material"_a = material);
}

py::SceneObject::SceneObject(mitsuba::ref<mi::Mesh> mesh, const std::string& name, const py::RadioMaterial& material)
{
    this->InitPythonClassCapability::init("mi_mesh"_a = mesh.get(), "name"_a = name, "radio_material"_a = material);
}

py::SceneObject::SceneObject(const std::string& fname, const std::string& name, const py::RadioMaterial& material)
{
    this->InitPythonClassCapability::init("fname"_a = fname, "name"_a = name, "radio_material"_a = material);
}

const char* py::SceneObject::className() const
{
    return "SceneObject";
}

std::string py::SceneObject::name() const
{
    return sionna::access<std::string>(bound_, "name");
}

typename mi::Point3f py::SceneObject::position() const
{
    return sionna::access<mi::Point3f>(bound_, "position");
}

typename mi::Point3f py::SceneObject::orientation() const
{
    return sionna::access<mi::Point3f>(bound_, "orientation");
}

typename mi::Vector3f py::SceneObject::scaling() const
{
    return sionna::access<mi::Vector3f>(bound_, "scaling");
}

typename mi::Vector3f py::SceneObject::velocity() const
{
    return sionna::access<mi::Vector3f>(bound_, "velocity");
}

mitsuba::ref<mi::Mesh> py::SceneObject::mesh() const
{
    return sionna::access<mitsuba::ref<mi::Mesh>>(bound_, "_mi_mesh");
}

py::RadioMaterial py::SceneObject::material() const
{
    return sionna::access<py::RadioMaterial>(bound_, "radio_material");
}

void py::SceneObject::setPosition(const mi::Point3f& position)
{
    sionna::set(bound_, "position", position);
}

void py::SceneObject::setOrientation(const mi::Point3f& orientation)
{
    sionna::set(bound_, "orientation", orientation);
}

void py::SceneObject::setScaling(const mi::Vector3f& scaling)
{
    sionna::set(bound_, "scaling", scaling);
}

void py::SceneObject::setVelocity(const mi::Vector3f& velocity)
{
    sionna::set(bound_, "velocity", velocity);
}

void py::SceneObject::setMaterial(const py::RadioMaterial& material)
{
    sionna::set(bound_, "radio_material", material);
}
