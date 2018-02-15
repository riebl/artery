/*
 * Artery V2X Simulation Framework
 * Copyright 2018 Christian Hagau
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef MULTITYPEMODULEMAPPER_H
#define MULTITYPEMODULEMAPPER_H

#include "traci/ModuleMapper.h"
#include <omnetpp/csimplemodule.h>
#include <tuple>
#include <vector>

namespace omnetpp {
    class cRNG;
    class cXMLElement;
} // namespace omnetpp

namespace traci
{

class MultiTypeModuleMapper : public ModuleMapper, public omnetpp::cSimpleModule
{
public:
	void initialize() override;
	omnetpp::cModuleType* vehicle(NodeManager&, const std::string&) override;

private:
	using VehicleType = std::tuple<omnetpp::cModuleType*, double>;

	void parseVehicleTypes(const omnetpp::cXMLElement*);

	double mCdfValue;
	std::vector<VehicleType> mVehicleTypes;
};

} // namespace traci

#endif /* MULTITYPEMODULEMAPPER_H */

