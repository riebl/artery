#ifndef OTS_GTUCREATIONPOLICY_H_FDXRVCJF
#define OTS_GTUCREATIONPOLICY_H_FDXRVCJF

#include "ots/GtuObject.h"
#include <omnetpp/cmodule.h>
#include <string>

namespace ots
{

class GtuCreationPolicy
{
public:
    class Instruction
    {
    public:
        void setModuleType(omnetpp::cModuleType* module_type) { m_module_type = module_type; }
        omnetpp::cModuleType* getModuleType() const { return m_module_type; }

        void setSinkPath(const std::string& path) { m_sink_path = path; }
        const char* getSinkPath() const { return m_sink_path.c_str(); }

    private:
        omnetpp::cModuleType* m_module_type = nullptr;
        std::string m_sink_path;
    };

    virtual Instruction getInstruction(const GtuObject&) = 0;
    virtual ~GtuCreationPolicy() = default;
};

} // namespace ots

#endif /* OTS_GTUCREATIONPOLICY_H_FDXRVCJF */

