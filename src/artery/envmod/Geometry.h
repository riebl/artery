/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ENVMOD_GEOMETRY_H_VKGEIFYB
#define ENVMOD_GEOMETRY_H_VKGEIFYB

#include "artery/utility/Geometry.h"
#include <boost/geometry/geometries/geometries.hpp>
#include <vector>

namespace artery
{
namespace geometry
{

using Point = boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>;
using Polygon = boost::geometry::model::polygon<Point, false, false, std::vector>;
using Box = boost::geometry::model::box<Point>;
using LineString = boost::geometry::model::linestring<Point>;
using Ring = boost::geometry::model::ring<Point, true, false>;

} // namespace geometry
} // namespace artery

#endif /* ENVMOD_GEOMETRY_H_VKGEIFYB */

