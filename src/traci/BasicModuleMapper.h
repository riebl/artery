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
    omnetpp::cModuleType* person(NodeManager&, const std::string&) override;

protected:
    virtual bool equipPerson();
    virtual bool equipVehicle();

private:
    omnetpp::cModuleType* m_person_type;
    omnetpp::cModuleType* m_vehicle_type;
    omnetpp::cRNG* m_rng;
    double m_person_penetration;
    double m_vehicle_penetration;
};

} // namespace traci

#endif /* BASICMODULEMAPPER_H_DPQ8TMFW */

