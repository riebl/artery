/*
 * Artery V2X Simulation Framework
 * Copyright 2019 Alexander Willecke, Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "traci/RegionsOfInterest.h"
#include <boost/geometry/algorithms/append.hpp>
#include <boost/geometry/algorithms/correct.hpp>
#include <boost/geometry/algorithms/within.hpp>
#include <boost/geometry/geometries/register/point.hpp>
#include <boost/lexical_cast.hpp>
#include <omnetpp/clog.h>

BOOST_GEOMETRY_REGISTER_POINT_2D(libsumo::TraCIPosition, double, cs::cartesian, x, y)

namespace traci
{

void RegionsOfInterest::initialize(const omnetpp::cXMLElement& regions, const Boundary& boundary)
{
    using omnetpp::cXMLElement;

    const Region boundary_region = RegionsOfInterest::buildRegion(boundary);
    for (cXMLElement* region : regions.getChildrenByTagName("polygon")) {
        Region poly;
        for (cXMLElement* point : region->getChildrenByTagName("point")) {
            /* iterate points in polygon */
            double x = boost::lexical_cast<double>(point->getAttribute("x"));
            double y = boost::lexical_cast<double>(point->getAttribute("y"));

            boost::geometry::append(poly, Point { x, y });
        }
        boost::geometry::correct(poly);

        if (boost::geometry::within(poly, boundary_region)) {
            m_regions.emplace_back(std::move(poly));
        } else {
            EV_STATICCONTEXT
            EV_WARN << "Region is out of scenario boundary!\n";
        }
    }
}

bool RegionsOfInterest::cover(const TraCIPosition& pos) const
{
    for (const auto& region : m_regions) {
        if (boost::geometry::within(pos, region)) {
            return true;
        }
    }

    return false;
}

RegionsOfInterest::Region RegionsOfInterest::buildRegion(const Boundary& boundary)
{
    using namespace boost::geometry;
    Region region;
    append(region, Point { boundary.lowerLeftPosition().x, boundary.lowerLeftPosition().y });
    append(region, Point { boundary.upperRightPosition().x, boundary.lowerLeftPosition().y });
    append(region, Point { boundary.upperRightPosition().x, boundary.upperRightPosition().y });
    append(region, Point { boundary.lowerLeftPosition().x, boundary.upperRightPosition().y });
    return region;
}

} // namespace traci
