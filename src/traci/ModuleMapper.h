#ifndef MODULEMAPPER_H_1CHFSD3N
#define MODULEMAPPER_H_1CHFSD3N

#include <omnetpp/cmodule.h>
#include <string>

namespace traci
{

class NodeManager;

class ModuleMapper
{
public:
    virtual omnetpp::cModuleType* person(NodeManager&, const std::string& id) = 0;
    virtual omnetpp::cModuleType* vehicle(NodeManager&, const std::string& id) = 0;
    virtual ~ModuleMapper() = default;
};

} // namespace traci

#endif /* MODULEMAPPER_H_1CHFSD3N */
