#ifndef BOUNDARY_H_5S0VZLYU
#define BOUNDARY_H_5S0VZLYU

#include "traci/sumo/utils/traci/TraCIAPI.h"
#include <stdexcept>

namespace traci
{

class Boundary
{
public:
    Boundary()
    {
        boundary.value.resize(2);
    }

    Boundary(const Boundary&) = default;
    Boundary& operator=(const Boundary&) = default;

    explicit Boundary(const libsumo::TraCIPositionVector& pv) : boundary(pv)
    {
        if (pv.value.size() != 2)
            throw std::runtime_error("TraCI boundary has to consist of exactly two positions");
    }

    operator const libsumo::TraCIPositionVector& () { return boundary; }

    const libsumo::TraCIPosition& lowerLeftPosition() const { return boundary.value[0]; }
    const libsumo::TraCIPosition& upperRightPosition() const { return boundary.value[1]; }

private:
    libsumo::TraCIPositionVector boundary;
};

} // namespace traci

#endif /* BOUNDARY_H_5S0VZLYU */

