#ifndef SRC_TRACI_TESTBEDMODULEMAPPER_H_
#define SRC_TRACI_TESTBEDMODULEMAPPER_H_

#include "traci/BasicModuleMapper.h"

namespace traci
{

class TestbedModuleMapper : public BasicModuleMapper
{
public:
    void initialize() override;
    omnetpp::cModuleType* vehicle(NodeManager& manager, const std::string& id) override;

private:
    std::string m_twinId;
    omnetpp::cModuleType* m_twinType;
};

} // namespace traci

#endif /* SRC_TRACI_TESTBEDMODULEMAPPER_H_ */
