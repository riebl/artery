#ifndef ARTERY_TRACISCENARIOMANAGERSTORYBOARD_H_QEMRAYD1
#define ARTERY_TRACISCENARIOMANAGERSTORYBOARD_H_QEMRAYD1

#include "veins/modules/mobility/traci/TraCIModuleMapper.h"
#include "veins/modules/mobility/traci/TraCIScenarioManagerLaunchd.h"
#include <memory>

namespace Veins {
class TraCIListener;
class TraCINodeManager;
}

class TraCIScenarioManagerArtery : public Veins::TraCIScenarioManagerBase
{
public:
    TraCIScenarioManagerArtery();
    virtual ~TraCIScenarioManagerArtery();
    void initialize(int stage) override;
    void handleSelfMsg(cMessage*) override;

    static const simsignal_t signalUpdateStep;

protected:
    void init_traci() override;

private:
    typedef Veins::TraCIScenarioManagerBase parent_class;
    Veins::TraCIModuleMapper m_mapper;
    std::unique_ptr<Veins::TraCINodeManager> m_nodes_base;
    std::unique_ptr<Veins::TraCINodeManager> m_nodes;
    cXMLElement* m_launch_config;
    int m_seed;
};

#endif /* ARTERY_TRACISCENARIOMANAGERSTORYBOARD_H_QEMRAYD1 */

