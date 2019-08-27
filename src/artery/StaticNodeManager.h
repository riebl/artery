#ifndef ARTERY_STATICNODEMANAGER_H_EZRAPHTB
#define ARTERY_STATICNODEMANAGER_H_EZRAPHTB

#include "artery/utility/Geometry.h"
#include <omnetpp/csimplemodule.h>
#include <list>
#include <map>
#include <string>

namespace artery
{

class StaticNodeManager : public omnetpp::cSimpleModule
{
public:
    static const omnetpp::simsignal_t addRoadSideUnitSignal;

    void initialize(int stage) override;
    int numInitStages() const override;

    struct RSU
    {
        Position position;
        std::list<double> antennaDirections;
    };

protected:
    virtual void loadRoadSideUnits();
    virtual void addRoadSideUnit(const std::string& index);
    virtual omnetpp::cModule* createRoadSideUnitModule(const std::string&);

private:
    bool mDirectional;
    std::string mRsuPrefix;
    std::map<std::string, RSU> mRsuMap;
};

} // namespace artery

#endif /* ARTERY_STATICNODEMANAGER_H_EZRAPHTB */

