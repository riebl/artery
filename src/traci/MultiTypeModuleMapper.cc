#include "traci/MultiTypeModuleMapper.h"
#include <omnetpp/ccomponenttype.h>
#include <omnetpp/cxmlelement.h>
#include <boost/lexical_cast.hpp>

namespace traci
{

Define_Module(MultiTypeModuleMapper)

void MultiTypeModuleMapper::initialize()
{
	auto personTypes = par("personTypes").xmlValue();
	parsePersonTypes(personTypes);

	auto vehicleTypes = par("vehicleTypes").xmlValue();
	parseVehicleTypes(vehicleTypes);
}

void MultiTypeModuleMapper::parsePersonTypes(const omnetpp::cXMLElement* personTypes)
{
	auto throwError = [](const std::string& msg, const omnetpp::cXMLElement* node) {
		throw omnetpp::cRuntimeError("%s: %s", node->getSourceLocation(), msg.c_str());
	};

	mPersonCdfValue = 0.0;
	mPersonTypes.clear();

	for (omnetpp::cXMLElement* personTag : personTypes->getChildrenByTagName("person")) {
		auto typeString = personTag->getAttribute("type");
		if (!typeString) {
			throwError("missing 'type' attribute in 'person' tag", personTag);
		}
		auto type = omnetpp::cModuleType::get(typeString);

		auto rateAttribute = personTag->getAttribute("rate");
		if (!rateAttribute) {
			throwError("missing 'rate' attribute in 'person' tag", personTag);
		}
		double rate = boost::lexical_cast<double>(rateAttribute);

		mPersonCdfValue += rate;
		mPersonTypes.push_back(std::make_tuple(type, mPersonCdfValue));
	}
}

void MultiTypeModuleMapper::parseVehicleTypes(const omnetpp::cXMLElement* vehicleTypes)
{
	auto throwError = [](const std::string& msg, const omnetpp::cXMLElement* node) {
		throw omnetpp::cRuntimeError("%s: %s", node->getSourceLocation(), msg.c_str());
	};

	mVehicleCdfValue = 0.0;
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

		mVehicleCdfValue += rate;
		mVehicleTypes.push_back(std::make_tuple(type, mVehicleCdfValue));
	}
}

omnetpp::cModuleType* MultiTypeModuleMapper::person(NodeManager& manager, const std::string& id)
{
	omnetpp::cModuleType* moduleType = nullptr;
	const double dice = uniform(0.0, mPersonCdfValue);

	for (PersonType& personType : mPersonTypes) {
		if (dice < std::get<1>(personType)) {
		    moduleType = std::get<0>(personType);
		    break;
		}
	}

	return moduleType;
}

omnetpp::cModuleType* MultiTypeModuleMapper::vehicle(NodeManager& manager, const std::string& id)
{
	omnetpp::cModuleType* moduleType = nullptr;
	const double dice = uniform(0.0, mVehicleCdfValue);

	for (VehicleType& vehicleType : mVehicleTypes) {
		if (dice < std::get<1>(vehicleType)) {
		    moduleType = std::get<0>(vehicleType);
		    break;
		}
	}

	return moduleType;
}

} // namespace traci
