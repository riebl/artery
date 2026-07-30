#pragma once

#include <artery/sionna/bridge/Fwd.h>
#include <artery/sionna/bridge/bindings/Scene.h>
#include <artery/sionna/bridge/bindings/SceneObject.h>
#include <artery/sionna/environment/api/SionnaAPI.h>
#include <traci/Boundary.h>

namespace artery::sionna
{

// Rotation/translation/remap based coordinate transform used between SUMO,
// canonical scene coordinates, and Sionna's local scene axis order.
class AffineCoordinateTransform : public ICoordinateTransformProxy
{
public:
    // Initialize transformer with remapping matrices, and
    // Sionna API.
    AffineCoordinateTransform(
        const mi::Matrix3f& remap, const mi::Matrix2f& rotation, const mi::Vector3f& translation, const traci::Boundary& boundary, ISionnaAPI* api);

    // ICoordinateTransformProxy implementation.
    mi::Point3f fromSumo(const libsumo::TraCIPosition& position) override;
    mi::Point3f fromSumo(traci::TraCIAngle heading) override;
    CoordinateArray3f toSionnaScene(const CoordinateArray3f& v) override;
    CoordinateArray3f fromSionnaScene(const CoordinateArray3f& v) override;
    void adjustVerticalComponent(py::SceneObject& object) override;

private:
    // Matrices used for Sionna transforms.
    mi::Matrix3f remap_;
    mi::Matrix3f inverseRemap_;
    mi::Matrix2f rotation_;
    mi::Matrix2f inverseRotation_;
    mi::Vector3f translation_;

    // Boundary is used for SUMO coordinate casts.
    traci::Boundary boundary_;
    ISionnaAPI* api_ = nullptr;
};

}  // namespace artery::sionna
