#pragma once

namespace artery::sionna
{

class ISceneVisualizer
{
public:
    virtual void renderFrame() = 0;

    virtual ~ISceneVisualizer() = default;
};

}  // namespace artery::sionna
