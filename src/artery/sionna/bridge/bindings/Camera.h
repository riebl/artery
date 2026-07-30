#pragma once

// for ref.
// clang-format off
#include <nanobind/nanobind.h>
// clang-format on

#include <artery/sionna/bridge/bindings/Modules.h>
#include <artery/sionna/bridge/capabilities/Calling.h>

namespace artery::sionna::py
{

class SIONNA_BRIDGE_API Camera : public SionnaRtModule, public InitPythonClassCapability
{
public:
    const char* className() const override;

    Camera() = default;

    // Wrap an existing Sionna Camera python object.
    explicit Camera(nanobind::object obj);

    // Construct a camera from Sionna-local position and Euler orientation.
    Camera(const mi::Point3f& position, const mi::Point3f& orientation = mi::Point3f(0.f, 0.f, 0.f));

    // Camera position in Sionna coordinates.
    mi::Point3f position() const;
    void setPosition(const mi::Point3f& position);

    // Camera orientation in Sionna Euler angles.
    mi::Point3f orientation() const;
    void setOrientation(const mi::Point3f& orientation);
};

}  // namespace artery::sionna::py
