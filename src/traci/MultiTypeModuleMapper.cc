#include "traci/MultiTypeModuleMapper.h"
#include <omnetpp/ccomponenttype.h>
#include <boost/lexical_cast.hpp>
#include <boost/exception/diagnostic_information.hpp>

namespace traci
{

Define_Module(MultiTypeModuleMapper)

void MultiTypeModuleMapper::initialize()
{
	mRng = getRNG(0);

	auto vehicleTypes =  par("vehicleTypes").xmlValue();
	parseVehicleTypes(vehicleTypes);
}

void MultiTypeModuleMapper::parseVehicleTypes(omnetpp::cXMLElement* vehicleTypes)
{
	auto throwError = [](std::string msg, omnetpp::cXMLElement* node) {
		std::ostringstream out;
		out << node->getSourceLocation() << ": " << msg;
		throw omnetpp::cRuntimeError(out.str().c_str());
	};

	double cdfValue = 0;
	for (omnetpp::cXMLElement* vehicleNode: vehicleTypes->getChildrenByTagName("vehicle")) {
		auto typeString = vehicleNode->getAttribute("type");
		if (!typeString) {
			throwError("missing 'type' attribute in 'vehicle' tag", vehicleNode);
		}

		auto type = omnetpp::cModuleType::get(typeString);

		auto penetrationTag = vehicleNode->getFirstChildWithTag("penetration");
		if (!penetrationTag) {
			throwError("missing 'penetration' tag", vehicleNode);
		}
		auto rateAttribute = penetrationTag->getAttribute("rate");
		if (!rateAttribute) {
			throwError("missing 'rate' attribute in 'penetration' tag", penetrationTag);
		}
		double rate;
		try {
			rate = boost::lexical_cast<double>(rateAttribute);
		} catch (boost::bad_lexical_cast& e) {
			throwError("bad 'rate' attribute value: could not cast to double", penetrationTag);
		}

		auto vehicleType = std::make_tuple(type, rate);
		mVehicleTypes.push_back(vehicleType);

		cdfValue += rate;
		mVehicleTypesCdfValues.push_back(cdfValue);
	}
}

omnetpp::cModuleType* MultiTypeModuleMapper::vehicle(NodeManager& manager, const std::string& id)
{
	const double dice = omnetpp::uniform(mRng, 0.0, 1.0);
	for (auto i = 0; i < mVehicleTypesCdfValues.size(); i++) {
		if (dice <= mVehicleTypesCdfValues[i]) {
			return std::get<0>(mVehicleTypes[i]);
		}
	}
	throw omnetpp::cRuntimeError("Unable to equip vehicle");
}

} // namespace traci
