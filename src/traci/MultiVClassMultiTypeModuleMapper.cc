#include "traci/MultiVClassMultiTypeModuleMapper.h"
#include "traci/VariableCache.h"
#include <omnetpp/ccomponenttype.h>
#include <omnetpp/cxmlelement.h>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <inet/common/ModuleAccess.h>

namespace traci
{

Define_Module(MultiVClassMultiTypeModuleMapper)

void MultiVClassMultiTypeModuleMapper::initialize()
{
	auto personTypes = par("personTypes").xmlValue();
	parsePersonTypes(personTypes);

	auto vehicleTypes = par("vehicleTypes").xmlValue();
	parseVehicleTypes(vehicleTypes);

	BasicNodeManager* manager = inet::getModuleFromPar<BasicNodeManager>(par("basicNodeManager"), this);
	m_subscription_manager = manager->getSubscriptions();
}

void MultiVClassMultiTypeModuleMapper::parseVehicleTypes(const omnetpp::cXMLElement* vehicleTypes)
{
	auto throwError = [](const std::string& msg, const omnetpp::cXMLElement* node) {
		throw omnetpp::cRuntimeError("%s: %s", node->getSourceLocation(), msg.c_str());
	};

	mVehicleTypes.clear();

	for (auto* v : vehicleTypes->getChildrenByTagName("vehicles")) {
		const std::string& vehicleClasses = v->getAttribute("vClass");

		std::vector<std::string> classes;
		boost::split(classes, vehicleClasses, boost::is_any_of(" "));

		for (auto& vehicleClass : classes) {
			double cdfValue = 0.0;

			if (mVehicleTypes.find(vehicleClass) == mVehicleTypes.end()) {
				//new class, allocate vector
				mVehicleTypes[vehicleClass] = std::make_pair(0.0, std::vector<VehicleType>());
			}

			for (omnetpp::cXMLElement* vehicleTag : v->getChildrenByTagName("vehicle")) {
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

				cdfValue += rate;
				mVehicleTypes[vehicleClass].second.push_back(std::make_tuple(type, cdfValue));
			}
			if (cdfValue != 1.0) {
				std::cout << std::endl << "vehicle rates do not add up to 1.0 in:" << std::endl
					<< v->getXML() << std::endl;
			}
			mVehicleTypes[vehicleClass].first = cdfValue;
		}
	}
}

void MultiVClassMultiTypeModuleMapper::parsePersonTypes(const omnetpp::cXMLElement* personTypes)
{
	auto throwError = [](const std::string& msg, const omnetpp::cXMLElement* node) {
		throw omnetpp::cRuntimeError("%s: %s", node->getSourceLocation(), msg.c_str());
	};

	mPersonTypes.clear();

	for (auto* p : personTypes->getChildrenByTagName("persons")) {
		const std::string& personClasses = p->getAttribute("vClass");

		std::vector<std::string> classes;
		boost::split(classes, personClasses, boost::is_any_of(" "));

		for (auto& personClass : classes) {
			double cdfValue = 0.0;

			if (mPersonTypes.find(personClass) == mPersonTypes.end()) {
				//new class, allocate vector
				mPersonTypes[personClass] = std::make_pair(0.0, std::vector<PersonType>());
			}

			for (omnetpp::cXMLElement* personTag : p->getChildrenByTagName("person")) {
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

				cdfValue += rate;
				mPersonTypes[personClass].second.push_back(std::make_tuple(type, cdfValue));
			}
			if (cdfValue != 1.0) {
				std::cout << std::endl << "person rates do not add up to 1.0 in:" << std::endl
					<< p->getXML() << std::endl;
			}
			mPersonTypes[personClass].first = cdfValue;
		}
	}
}

omnetpp::cModuleType* MultiVClassMultiTypeModuleMapper::vehicle(NodeManager& manager, const std::string& id)
{
	omnetpp::cModuleType* moduleType = nullptr;

	auto& vehicle = *m_subscription_manager->getVehicleCache(id);
	auto vehicleClass = vehicle.get<libsumo::VAR_VEHICLECLASS>();

	auto vehicleTypeList = mVehicleTypes.find(vehicleClass);
	if (vehicleTypeList != mVehicleTypes.end()) {
		double vehicleCdf = vehicleTypeList->second.first;
		auto vehicleTypes = vehicleTypeList->second.second;
		const double dice = uniform(0.0, vehicleCdf);
		for (VehicleType& vehicleType : vehicleTypes) {
			if (dice < std::get<1>(vehicleType)) {
				moduleType = std::get<0>(vehicleType);
				break;
			}
		}
	} else {
		std::cout << "encountered unknown vehicleClass: " << vehicleClass << std::endl;
	}

	return moduleType;
}

omnetpp::cModuleType* MultiVClassMultiTypeModuleMapper::person(NodeManager& manager, const std::string& id)
{
	omnetpp::cModuleType* moduleType = nullptr;

	auto& person = *m_subscription_manager->getPersonCache(id);
	auto personClass = person.get<libsumo::VAR_VEHICLECLASS>();

	auto personTypeList = mPersonTypes.find(personClass);
	if (personTypeList != mPersonTypes.end()) {
		double personCdf = personTypeList->second.first;
		auto personTypes = personTypeList->second.second;
		const double dice = uniform(0.0, personCdf);
		for (PersonType& personType : personTypes) {
			if (dice < std::get<1>(personType)) {
				moduleType = std::get<0>(personType);
				break;
			}
		}
	} else {
		std::cout << "encountered unknown personClass: " << personClass << std::endl;
	}

	return moduleType;
}

} // namespace traci
