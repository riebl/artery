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
namespace traci { class VehicleController; }

namespace artery
{

class FilterRules
{
public:
    using Filter = std::function<bool()>;

    FilterRules(omnetpp::cRNG* rng, const traci::VehicleController& vehicle);
    virtual bool applyFilterConfig(const omnetpp::cXMLElement&);

protected:
    Filter createVehicleFilterNamePattern(const omnetpp::cXMLElement&) const;
    Filter createVehicleFilterPenetrationRate(const omnetpp::cXMLElement&) const;

private:
    omnetpp::cRNG* mRNG;
    const traci::VehicleController& mVehicleController;
};

} // namespace artery

#endif /* FILTERRULES_H_UZBNGKZV */
