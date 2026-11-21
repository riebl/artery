#include "Ground.h"

// FIXME: Should go when INET is updated. It acts a big
// aggressive with defining global scope macros.
#include <artery/sionna/bridge/Helpers.h>
#include <artery/sionna/environment/api/SionnaAPI.h>
#include <drjit/array_router.h>
#include <mitsuba/render/scene.h>

#include <limits>

using namespace artery::sionna;

Ground::Ground(ISionnaAPI* api) : api_(api)
{
}

double Ground::getElevation(const inet::Coord& position) const
{
    auto scene = api_->miScene();

    const auto origin = mi::Point3f(position.x, position.y, api_->worldMax().z() + 1e-3f);
    const auto down = mi::Vector3f(0.0f, 0.0f, -1.0f);
    const auto interaction = scene->ray_intersect(mi::Ray3f(origin, down));

    if (!drjit::all_nested(interaction.is_valid())) {
        return std::numeric_limits<double>::quiet_NaN();
    }

    return toScalar<double>(interaction.p.z());
}
