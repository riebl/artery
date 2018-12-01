/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/utility/Identity.h"
#include "artery/utility/FilterRules.h"
#include "artery/utility/PointerCheck.h"
#include <boost/lexical_cast.hpp>
#include <omnetpp/ccomponenttype.h>
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

FilterRules::FilterRules(omnetpp::cRNG* rng, const Identity& id) :
    mRNG(rng), mIdentity(id)
{
}

auto FilterRules::createFilterNamePattern(const cXMLElement& name_filter_cfg) const -> Filter
{
    const char* name_pattern = name_filter_cfg.getAttribute("pattern");
    const char* name_match = name_filter_cfg.getAttribute("match");
    bool inverse = name_match && std::strcmp(name_match, "inverse") == 0;
    if (!name_pattern) {
        throw cRuntimeError("Required pattern attribute is missing for name filter");
    }

    std::regex name_regex(name_pattern);
    Filter name_filter = [this, name_regex, inverse]() {
            return std::regex_match(mIdentity.traci, name_regex) ^ inverse;
    };
    return name_filter;
}

auto FilterRules::createFilterPenetrationRate(const cXMLElement& penetration_filter_cfg) const -> Filter
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

auto FilterRules::createFilterTypePattern(const cXMLElement& type_filter_cfg) const -> Filter
{
    const char* type_pattern = type_filter_cfg.getAttribute("pattern");
    const char* type_match = type_filter_cfg.getAttribute("match");
    bool inverse = type_match && std::strcmp(type_match, "inverse") == 0;
    if (!type_pattern) {
        throw cRuntimeError("Required pattern attribute is missing for type filter");
    }

    double type_rate = 1.0;
    const char* type_rate_str = type_filter_cfg.getAttribute("rate");
    if (type_rate_str) {
        type_rate = boost::lexical_cast<double>(type_rate_str);
    }
    if (type_rate > 1.0 || type_rate < 0.0) {
        throw cRuntimeError("Type penetration rate is out of range [0.0, 1.0]");
    }

    std::regex type_regex(type_pattern);
    Filter type_filter = [this, type_rate, type_regex, inverse]() {
        auto rate_predicate = type_rate >= uniform(mRNG, 0.0, 1.0);
        auto type = notNullPtr(mIdentity.host)->getModuleType()->getFullName();
        return (std::regex_match(type, type_regex) && rate_predicate) ^ inverse;
    };
    return type_filter;
}

bool FilterRules::applyFilterConfig(const omnetpp::cXMLElement& filter_cfg)
{
    std::list<Filter> filters;

    cXMLElementList name_filter_cfg_list = filter_cfg.getChildrenByTagName("name");
    for (cXMLElement* cfg : name_filter_cfg_list) {
        filters.emplace_back(createFilterNamePattern(*cfg));
    }

    cXMLElement* penetration_filter_cfg = filter_cfg.getFirstChildWithTag("penetration");
    if (penetration_filter_cfg) {
        filters.emplace_back(createFilterPenetrationRate(*penetration_filter_cfg));
    }

    cXMLElementList type_filter_cfg_list = filter_cfg.getChildrenByTagName("type");
    for (cXMLElement* cfg : type_filter_cfg_list) {
        filters.emplace_back(createFilterTypePattern(*cfg));
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
