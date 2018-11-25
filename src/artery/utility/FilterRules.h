/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef FILTERRULES_H_UZBNGKZV
#define FILTERRULES_H_UZBNGKZV

#include <functional>

// forward declarations
namespace omnetpp {
    class cRNG;
    class cXMLElement;
}

namespace artery
{

// forward declaration
class Identity;

class FilterRules
{
public:
    using Filter = std::function<bool()>;

    FilterRules(omnetpp::cRNG* rng, const Identity& id);
    virtual bool applyFilterConfig(const omnetpp::cXMLElement&);

protected:
    Filter createFilterNamePattern(const omnetpp::cXMLElement&) const;
    Filter createFilterPenetrationRate(const omnetpp::cXMLElement&) const;
    Filter createFilterTypePattern(const omnetpp::cXMLElement&) const;

private:
    omnetpp::cRNG* mRNG;
    const Identity& mIdentity;
};

} // namespace artery

#endif /* FILTERRULES_H_UZBNGKZV */
