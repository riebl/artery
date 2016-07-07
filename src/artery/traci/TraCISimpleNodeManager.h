#ifndef TRACISIMPLENODEMANAGER_H_CROHKRE7
#define TRACISIMPLENODEMANAGER_H_CROHKRE7

#include "veins/modules/mobility/traci/TraCINodeManager.h"
#include <boost/optional.hpp>
#include <map>
#include <string>

namespace Veins {
class TraCIMobility;
class TraCIModuleMapper;
} // namespace Veins

class TraCISimpleNodeManager : public Veins::TraCINodeManager
{
	public:
		TraCISimpleNodeManager(const Veins::TraCIModuleMapper&, cModule* parent);
		void setMobilityModule(cPar&);

		void add(const std::string& id, const NodeData&, const std::string& vtype, simtime_t start);
		void remove(const std::string& id);
		void update(const std::string& id, const NodeData&);
		cModule* get(const std::string& id);
		void finish();
		size_t size();

        protected:
		virtual void preInitialize(cModule*, const std::string& nodeId, const NodeData&);
		virtual void postInitialize(cModule*, const std::string& nodeId, const NodeData&);
		Veins::TraCIMobility* getMobility(cModule*);

	private:
		const Veins::TraCIModuleMapper& m_moduleMapper;
		cModule* m_moduleParent;
		unsigned m_moduleIndex;
		boost::optional<cPar> m_moduleMobility;
		std::map<std::string, cModule*> m_nodes;
};

#endif /* TRACISIMPLENODEMANAGER_H_CROHKRE7 */

