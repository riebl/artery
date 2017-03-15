/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/traci/VehicleController.h"
#include "artery/utility/FilterRules.h"
#include <boost/lexical_cast.hpp>
#include <omnetpp/cexception.h>
#include <omnetpp/cxmlelement.h>
#include <omnetpp/distrib.h>
#include <algorithm>
#include <cstring>
#include <list>
#include <regex>

using namespace omnetpp;

namespace artery
{

FilterRules::FilterRules(omnetpp::cRNG* rng, const traci::VehicleController& vehicle) :
    mRNG(rng), mVehicleController(vehicle)
{
}

auto FilterRules::createVehicleFilterNamePattern(const cXMLElement& name_filter_cfg) const -> Filter
{
    const char* name_pattern = name_filter_cfg.getAttribute("pattern");
    const char* name_match = name_filter_cfg.getAttribute("match");
    bool inverse = name_match && std::strcmp(name_match, "inverse") == 0;
    if (!name_pattern) {
        throw cRuntimeError("Required pattern attribute is missing for name filter");
    }

    std::regex name_regex(name_pattern);
    Filter name_filter = [this, name_regex, inverse]() {
            return std::regex_match(mVehicleController.getVehicleId(), name_regex) ^ inverse;
    };
    return name_filter;
}

auto FilterRules::createVehicleFilterPenetrationRate(const cXMLElement& penetration_filter_cfg) const -> Filter
{
    const char* penetration_rate_str = penetration_filter_cfg.getAttribute("rate");
    if (!penetration_rate_str) {
        throw cRuntimeError("Required rate attribute is missing for penetration filter");
    }

    auto penetration_rate = boost::lexical_cast<double>(penetration_rate_str);
    if (penetration_rate > 1.0 || penetration_rate < 0.0) {
        throw cRuntimeError("Penetration rate is out of range [0.0, 1.0]");
    }

    Filter penetration_filter = [this, penetration_rate]() {
        return penetration_rate >= uniform(mRNG, 0.0, 1.0);
    };
    return penetration_filter;
}

bool FilterRules::applyFilterConfig(const omnetpp::cXMLElement& filter_cfg)
{
    std::list<Filter> filters;

    cXMLElement* name_filter_cfg = filter_cfg.getFirstChildWithTag("name");
    if (name_filter_cfg) {
        filters.emplace_back(createVehicleFilterNamePattern(*name_filter_cfg));
    }

    cXMLElement* penetration_filter_cfg = filter_cfg.getFirstChildWithTag("penetration");
    if (penetration_filter_cfg) {
        filters.emplace_back(createVehicleFilterPenetrationRate(*penetration_filter_cfg));
    }

    bool applicable = true;
    if (!filters.empty()) {
        const char* filter_operator = filter_cfg.getAttribute("operator") ? filter_cfg.getAttribute("operator") : "or";
        auto filter_executor = [](const Filter& filter) { return filter(); };
        if (std::strcmp(filter_operator, "or") == 0) {
            applicable = std::any_of(filters.begin(), filters.end(), filter_executor);
        } else if (std::strcmp(filter_operator, "and") == 0) {
            applicable = std::all_of(filters.begin(), filters.end(), filter_executor);
        } else {
            throw cRuntimeError("Unsupported filter operator: %s", filter_operator);
        }
    }
    return applicable;
}

} // namespace artery
