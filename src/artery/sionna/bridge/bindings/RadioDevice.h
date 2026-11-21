#pragma once

// for ref.
// clang-format off
#include <nanobind/nanobind.h>
// clang-format on

#include <artery/sionna/bridge/Compat.h>
#include <artery/sionna/bridge/bindings/Modules.h>
#include <artery/sionna/bridge/capabilities/Calling.h>

#include <optional>
#include <string>
#include <tuple>

namespace artery::sionna::py
{

// Base class for tx and rx (radio) devices. Note, these are Sionna's abstract entities that
// are not part of the scene, which makes adding or removing them cheap.
class SIONNA_BRIDGE_API RadioDevice : public SionnaRtModule, public InitPythonClassCapability
{
public:
    // Local alias for color.
    using TColor = std::tuple<float, float, float>;

    // Default constructor, does not hold valid object yet.
    RadioDevice() = default;
    // Assume passed object is of type RadioDevice.
    explicit RadioDevice(nanobind::object obj);

    // Access ID for this device.
    std::string name() const;
    // Access current pseudo-physical position for this device.
    mi::Point3f position() const;
    // Access orientation vector this device.
    mi::Point3f orientation() const;
    // Access velocity for this device.
    mi::Vector3f velocity() const;
    // Access color for this device. Useful for renders.
    TColor color() const;

    // Sets position for this device.
    void setPosition(const mi::Point3f& position);
    // Sets orientation for this device.
    void setOrientation(const mi::Point3f& orientation);
    // Sets velocity for this device. Used for path loss computation.
    void setVelocity(const mi::Vector3f& velocity);
    // Sets color for this device. Useful for renders.
    void setColor(TColor color);

    // Adjust orientation for this object like it would be
    // "looking" at target point.
    void lookAt(const mi::Point3f& target);
};

// Class wrapping Sionna transmitter.
class SIONNA_BRIDGE_API Transmitter : public RadioDevice
{
public:
    // IPythonClassIdentityCapability implementation.
    const char* className() const override;

    // Default constructor.
    Transmitter() = default;
    // Assume passed object is of type Transmitter.
    explicit Transmitter(nanobind::object obj);

    // Create new transmission with certain ID, scene position
    // and velocity.
    Transmitter(
        const std::string& name, const mi::Point3f& position, std::optional<mi::Point3f> orientation = std::nullopt,
        std::optional<mi::Vector3f> velocity = std::nullopt);

    // Get tx power in W.
    maybe_diff_t<mi::Float> power() const;
    // Get tx power in Dbm.
    maybe_diff_t<mi::Float> powerDbm() const;

    // Set tx power in dbm.
    void setPowerDbm(maybe_diff_t<mi::Float> powerDbm);
};

// Class wrapping Sionna receiver.
class SIONNA_BRIDGE_API Receiver : public RadioDevice
{
public:
    // IPythonClassIdentityCapability implementation.
    const char* className() const override;

    // Default constructor.
    Receiver() = default;
    // Assume passed object is of type Receiver.
    explicit Receiver(nanobind::object obj);

    // Create a receiver with certain ID, scene position and velocity.
    Receiver(
        const std::string& name, const mi::Point3f& position, std::optional<mi::Point3f> orientation = std::nullopt,
        std::optional<mi::Vector3f> velocity = std::nullopt);
};

}  // namespace artery::sionna::py
