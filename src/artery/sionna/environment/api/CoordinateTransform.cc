#include "CoordinateTransform.h"

#include <artery/sionna/bridge/Compat.h>
#include <artery/sionna/environment/Compat.h>
#include <artery/sionna/environment/api/SionnaAPI.h>
#include <artery/traci/Cast.h>
#include <drjit/array.h>
#include <drjit/array_router.h>
#include <libsumo/TraCIDefs.h>
#include <mitsuba/core/spectrum.h>
#include <mitsuba/render/mesh.h>
#include <mitsuba/render/scene.h>
#include <omnetpp/cexception.h>

#include <cmath>

using namespace artery::sionna;

AffineCoordinateTransform::AffineCoordinateTransform(
    const mi::Matrix3f& remap, const mi::Matrix2f& rotation, const mi::Vector3f& translation, const traci::Boundary& boundary, ISionnaAPI* api) :
    remap_(remap), rotation_(rotation), translation_(translation), api_(api), boundary_(boundary)
{
    auto check = [](const auto& m) {
        auto det = toScalar<double>(drjit::det(m));
        if (std::fabs(det) < 1E-12) {
            throw omnetpp::cRuntimeError("matrix is singular");
        }
    };

    check(remap);
    check(rotation);

    inverseRemap_ = drjit::inverse(remap_);
    inverseRotation_ = drjit::inverse(rotation_);
}

mi::Point3f AffineCoordinateTransform::fromSumo(const libsumo::TraCIPosition& position)
{
    const auto rotated = rotation_ * mi::Vector2f(fromScalar<mi::Float>(position.x), fromScalar<mi::Float>(position.y));
    const auto canonical = mi::Point3f(rotated.x(), rotated.y(), fromScalar<mi::Float>(position.z)) + translation_;
    return toSionnaScene(canonical);
}

mi::Point3f AffineCoordinateTransform::fromSumo(traci::TraCIAngle heading)
{
    return toSionnaScene(convert<mi::Point3f>(heading));
}

CoordinateArray3f AffineCoordinateTransform::toSionnaScene(const CoordinateArray3f& v)
{
    return remap_ * v;
}

CoordinateArray3f AffineCoordinateTransform::fromSionnaScene(const CoordinateArray3f& v)
{
    return inverseRemap_ * v;
}

void AffineCoordinateTransform::adjustVerticalComponent(py::SceneObject& object)
{
    auto base = fromSionnaScene(object.position());

    const auto mesh = object.mesh();
    const auto min = fromSionnaScene(mesh->bbox().min);
    const auto max = fromSionnaScene(mesh->bbox().max);

    // Take the centred point, maxed out by Z coordinate.
    auto origin = fromSionnaScene(mesh->bbox().center());
    origin.z() += (max.z() - min.z()) * 0.5;
    const auto bottomOffset = base.z() - min.z();

    if (api_ == nullptr) {
        return;
    }

    // Cast ray down, casting coordinates to Sionna first.
    auto scene = api_->miScene();
    const auto down = mi::Vector3f(0.0, 0.0, -1.0);
    auto ray = mi::Ray3f(toSionnaScene(origin), toSionnaScene(down));

    auto interaction = scene->ray_intersect(ray);
    bool foundSupport = false;
    for (int attempts = 0; attempts < 16; ++attempts) {
        if (!drjit::all_nested(interaction.is_valid())) {
            break;
        }

        if (!drjit::all_nested(interaction.shape == static_cast<const mi::Shape*>(mesh.get()))) {
            foundSupport = true;
            break;
        }

        ray.o = interaction.p + ray.d * fromScalar<mi::Float>(1e-3);
        interaction = scene->ray_intersect(ray);
    }

    // No interaction means we could not find support for object.
    if (!foundSupport) {
        EV_DEBUG << "Skipping vertical adjustment for " << object.name() << ": no support hits found"
                 << ", base=" << base;
        return;
    }

    // Interaction means we place object on hit point, shifting by bbox size.
    const auto hit = fromSionnaScene(interaction.p);
    auto pos = CoordinateArray3f(base.x(), base.y(), hit.z() + bottomOffset);

    EV_DEBUG << "Adjusted vertical component for " << object.name() << ": base=" << base << ", rayOrigin=" << origin << ", hit=" << hit
             << ", bottomOffset=" << bottomOffset << ", position=" << pos << ", down=" << down << "\n";

    object.setPosition(toSionnaScene(pos));
}
