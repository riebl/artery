#ifndef ARTERY_STATICNODEMANAGER_H_EZRAPHTB
#define ARTERY_STATICNODEMANAGER_H_EZRAPHTB

#include "artery/utility/Geometry.h"
#include <omnetpp/csimplemodule.h>
#include <map>
#include <string>
#include <vector>

namespace artery
{

class StaticNodeManager : public omnetpp::cSimpleModule
{
public:
    static const omnetpp::simsignal_t addRoadSideUnitSignal;

    StaticNodeManager();
    virtual ~StaticNodeManager();

protected:
    struct RSU
    {
        Position position;
        std::vector<double> antennaDirections;
    };

    void initialize(int stage) override;
    int numInitStages() const override;
    void handleMessage(omnetpp::cMessage*) override;

    virtual void loadRoadSideUnits();
    virtual void addRoadSideUnit(const std::string& index);
    virtual omnetpp::cModule* createRoadSideUnitModule(const std::string&);
    virtual void scheduleInsertionEvent();

private:
    omnetpp::cMessage* mInsertionEvent;
    std::multimap<omnetpp::SimTime, std::string> mInsertionQueue;
    bool mDirectionalAntennas;
    std::string mRsuPrefix;
    std::map<std::string, RSU> mRsuMap;
};

} // namespace artery

#endif /* ARTERY_STATICNODEMANAGER_H_EZRAPHTB */

