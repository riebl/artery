/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_ENVMOD_PRESELECTIONPOLYGON_H_ZLX9YEXN
#define ARTERY_ENVMOD_PRESELECTIONPOLYGON_H_ZLX9YEXN

#include "artery/envmod/PreselectionMethod.h"

namespace artery
{

/**
 * Preselecting the objects in sensor cone using polygon intersection checks
 */
class PreselectionPolygon : public PreselectionMethod
{
public:
    using PreselectionMethod::PreselectionMethod;

    void update() override;
    std::vector<std::string> select(const EnvironmentModelObject& ego, const SensorConfigFov&) override;
};

} // namespace artery

#endif /* ARTERY_ENVMOD_PRESELECTIONPOLYGON_H_ZLX9YEXN */

