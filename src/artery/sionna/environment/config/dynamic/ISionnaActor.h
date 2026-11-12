#pragma once

#include <artery/sionna/bridge/SionnaBridge.h>

namespace artery::sionna
{

class ISionnaActor
{
public:
    virtual ~ISionnaActor() = default;

    // SUMO identifier backing this scene actor.
    virtual const std::string& sumoId() const = 0;

    // Actor transform in canonical Sionna-local coordinates, i.e. before
    // final scene-axis remapping and with Z as the vertical component.
    virtual mi::Point3f position() const = 0;
    virtual mi::Point3f orientation() const = 0;
    virtual mi::Vector3f velocity() const = 0;

    // Mesh used by the represented scene object.
    virtual mitsuba::ref<mi::Mesh> mesh() const = 0;
};

}  // namespace artery::sionna
