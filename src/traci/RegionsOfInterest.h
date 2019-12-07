/*
 * Artery V2X Simulation Framework
 * Copyright 2019 Alexander Willecke, Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef REGIONSOFINTEREST_H_FT3XYZKP
#define REGIONSOFINTEREST_H_FT3XYZKP

#include "traci/Boundary.h"
#include "traci/Position.h"
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <omnetpp/cxmlelement.h>
#include <list>

namespace traci
{

class RegionsOfInterest
{
public:
    using Point = boost::geometry::model::d2::point_xy<double>;
    using Region = boost::geometry::model::polygon<Point>;

    RegionsOfInterest() = default;
    void initialize(const omnetpp::cXMLElement&, const Boundary&);
    bool cover(const TraCIPosition&) const;
    std::size_t size() const { return m_regions.size(); }
    bool empty() const { return m_regions.empty(); }

private:
    std::list<Region> m_regions;

    static Region buildRegion(const Boundary&);
};

} // namespace traci

#endif /* REGIONSOFINTEREST_H_FT3XYZKP */
