/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/envmod/PreselectionRtree.h"
#include "artery/envmod/Geometry.h"
#include <boost/geometry/algorithms/is_valid.hpp>
#include <omnetpp/cexception.h>

namespace artery
{

void PreselectionRtree::update()
{
    mRtree.clear();

    for (const auto& object : mObjects) {
        const auto& object_shape = object->getOutline();
        auto box = boost::geometry::return_envelope<geometry::Box>(object_shape);
        mRtree.insert(std::make_pair(box, object->getExternalId()));
    }
}

std::vector<std::string> PreselectionRtree::select(const EnvironmentModelObject& ego, const SensorConfigRadar& config)
{
    auto cone = createSensorArc(config, ego);
    if (!boost::geometry::is_valid(cone)) {
        throw omnetpp::cRuntimeError("polygon of sensor cone is invalid");
    }

    // Boost 1.58 accepted "cone" for intersection query. Fails for obscure reasons with 1.60+
    geometry::Polygon cone_copy;
    boost::geometry::convert(cone, cone_copy);
    std::vector<rtree_value> query_result;
    mRtree.query(boost::geometry::index::intersects(cone_copy), std::back_inserter(query_result));

    std::vector<std::string> objectsInSearchArea;
    for (const auto& intersection : query_result) {
        if (intersection.second != ego.getExternalId()) {
            objectsInSearchArea.push_back(intersection.second);
        }
    }
    return objectsInSearchArea;
}

} // namespace artery
