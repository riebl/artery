#ifndef SRC_TRACI_TESTBEDNODEMANAGER_H_
#define SRC_TRACI_TESTBEDNODEMANAGER_H_

#include "traci/BasicNodeManager.h"
#include <string>

namespace traci
{

class TestbedNodeManager : public BasicNodeManager
{
public:
    void initialize() override;

protected:
    virtual omnetpp::cModule* createModule(const std::string&, omnetpp::cModuleType*) override;

private:
    std::string m_twinId;
    std::string m_twinName;
};

} /* namespace traci */

#endif /* SRC_TRACI_TESTBEDNODEMANAGER_H_ */
