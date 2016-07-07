#ifndef ARTERY_TRACISCENARIOMANAGERSTORYBOARD_H_QEMRAYD1
#define ARTERY_TRACISCENARIOMANAGERSTORYBOARD_H_QEMRAYD1

#include "veins/modules/mobility/traci/TraCIScenarioManagerLaunchd.h"

namespace Veins {
class TraCIListener;
class TraCINodeManager;
}

class TraCIScenarioManagerArtery : public Veins::TraCIScenarioManagerLaunchd
{
public:
    TraCIScenarioManagerArtery();
    virtual ~TraCIScenarioManagerArtery();
    void initialize(int stage) override;
    void handleSelfMsg(cMessage*) override;

    static const simsignal_t signalUpdateStep;

private:
    typedef Veins::TraCIScenarioManagerLaunchd parent_class;
    Veins::TraCINodeManager* nodes;
};

#endif /* ARTERY_TRACISCENARIOMANAGERSTORYBOARD_H_QEMRAYD1 */

