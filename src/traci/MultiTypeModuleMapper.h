#ifndef MULTITYPEMODULEMAPPER_H
#define MULTITYPEMODULEMAPPER_H

#include "traci/ModuleMapper.h"
#include <omnetpp/csimplemodule.h>
#include <omnetpp/cxmlelement.h>
#include <omnetpp/crng.h>

#include <vector>

namespace traci
{

class MultiTypeModuleMapper : public ModuleMapper, public omnetpp::cSimpleModule
{
public:
	void initialize() override;
	omnetpp::cModuleType* vehicle(NodeManager&, const std::string&) override;

private:
	using VehicleType = std::tuple<omnetpp::cModuleType*, double>;
	using VehicleTypes = std::vector<VehicleType>;

	void parseVehicleTypes(omnetpp::cXMLElement*);

	VehicleTypes mVehicleTypes;
	std::vector<double> mVehicleTypesCdfValues;

	omnetpp::cRNG* mRng;
};

} // namespace traci

#endif /* MULTITYPEMODULEMAPPER_H */

