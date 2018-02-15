#include "traci/MultiTypeModuleMapper.h"
#include <omnetpp/ccomponenttype.h>
#include <omnetpp/cxmlelement.h>
#include <boost/lexical_cast.hpp>

namespace traci
{

Define_Module(MultiTypeModuleMapper)

void MultiTypeModuleMapper::initialize()
{
	auto vehicleTypes = par("vehicleTypes").xmlValue();
	parseVehicleTypes(vehicleTypes);
}

void MultiTypeModuleMapper::parseVehicleTypes(const omnetpp::cXMLElement* vehicleTypes)
{
	auto throwError = [](const std::string& msg, const omnetpp::cXMLElement* node) {
		throw omnetpp::cRuntimeError("%s: %s", node->getSourceLocation(), msg.c_str());
	};

	mCdfValue = 0.0;
	mVehicleTypes.clear();

	for (omnetpp::cXMLElement* vehicleTag : vehicleTypes->getChildrenByTagName("vehicle")) {
		auto typeString = vehicleTag->getAttribute("type");
		if (!typeString) {
			throwError("missing 'type' attribute in 'vehicle' tag", vehicleTag);
		}
		auto type = omnetpp::cModuleType::get(typeString);

		auto rateAttribute = vehicleTag->getAttribute("rate");
		if (!rateAttribute) {
			throwError("missing 'rate' attribute in 'vehicle' tag", vehicleTag);
		}
		double rate = boost::lexical_cast<double>(rateAttribute);

		mCdfValue += rate;
		mVehicleTypes.push_back(std::make_tuple(type, mCdfValue));
	}
}

omnetpp::cModuleType* MultiTypeModuleMapper::vehicle(NodeManager& manager, const std::string& id)
{
	omnetpp::cModuleType* moduleType = nullptr;
	const double dice = uniform(0.0, mCdfValue);

	for (VehicleType& vehicleType : mVehicleTypes) {
		if (dice < std::get<1>(vehicleType)) {
		    moduleType = std::get<0>(vehicleType);
		    break;
		}
	}

	return moduleType;
}

} // namespace traci
