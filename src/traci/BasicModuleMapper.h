#ifndef BASICMODULEMAPPER_H_DPQ8TMFW
#define BASICMODULEMAPPER_H_DPQ8TMFW

#include "traci/ModuleMapper.h"
#include <omnetpp/csimplemodule.h>
#include <omnetpp/crng.h>

namespace traci
{

class BasicModuleMapper : public ModuleMapper, public omnetpp::cSimpleModule
{
public:
    void initialize() override;
    omnetpp::cModuleType* vehicle(NodeManager&, const std::string&) override;

protected:
    virtual bool equipVehicle();

private:
    omnetpp::cModuleType* m_type;
    omnetpp::cRNG* m_rng;
    double m_penetration;
};

} // namespace traci

#endif /* BASICMODULEMAPPER_H_DPQ8TMFW */

