/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_ENVMOD_PRESELECTIONRTREE_H_JDC8PJFN
#define ARTERY_ENVMOD_PRESELECTIONRTREE_H_JDC8PJFN

#include "artery/envmod/PreselectionMethod.h"
#include <boost/geometry/index/rtree.hpp>

namespace artery
{

class PreselectionRtree : public PreselectionMethod
{
public:
    using PreselectionMethod::PreselectionMethod;

    void update() override;
    std::vector<std::string> select(const EnvironmentModelObject& ego, const SensorConfigFov&) override;

private:
    using rtree_value = std::pair<geometry::Box, std::string>;
    boost::geometry::index::rtree<rtree_value, boost::geometry::index::rstar<32>> mRtree;
};

} // namespace artery

#endif /* ARTERY_ENVMOD_PRESELECTIONRTREE_H_JDC8PJFN */
