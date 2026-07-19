#pragma once

#include <inet/environment/contract/IGround.h>

namespace artery::sionna
{

class ISionnaAPI;

class Ground : public inet::physicalenvironment::IGround
{
public:
    explicit Ground(ISionnaAPI* api);

    double getElevation(const inet::Coord& position) const override;

private:
    ISionnaAPI* api_;
};

}  // namespace artery::sionna
