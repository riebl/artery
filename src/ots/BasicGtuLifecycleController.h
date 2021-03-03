#ifndef OTS_BASICGTULIFECYCLECONTROLLER_H_FM8ENVOH
#define OTS_BASICGTULIFECYCLECONTROLLER_H_FM8ENVOH

#include "ots/GtuCreationPolicy.h"
#include "ots/GtuObject.h"
#include "ots/GtuSink.h"
#include <omnetpp/clistener.h>
#include <omnetpp/csimplemodule.h>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>

namespace ots
{

class BasicGtuLifecycleController : public omnetpp::cListener, public omnetpp::cSimpleModule
{
public:
    void initialize() override;
    void handleMessage(omnetpp::cMessage*) override;
    void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t, bool, omnetpp::cObject* = nullptr) override;
    void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t, const char*, omnetpp::cObject* = nullptr) override;
    void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t, omnetpp::cObject*, omnetpp::cObject* = nullptr) override;

protected:
    using Initializer = std::function<void(omnetpp::cModule*)>;

    virtual void addGtu(const std::string&);
    virtual void removeGtu(const std::string&);
    virtual void updateGtu(const GtuObject&);

    virtual omnetpp::cModule* addModule(const std::string&, omnetpp::cModuleType*, Initializer&);
    virtual void removeModule(const std::string&);
    virtual omnetpp::cModule* getModule(const std::string&);
    virtual GtuSink* getSink(const std::string&);
    virtual void createSink(const GtuObject&);

private:
    unsigned m_node_index = 0;
    std::map<std::string, omnetpp::cModule*> m_nodes;
    std::map<std::string, GtuSink*> m_gtu_sinks;
    std::unordered_map<std::string, std::unique_ptr<GtuObject>> m_pending_gtus;
    GtuCreationPolicy* m_creation_policy = nullptr;
};

} // namespace ots

#endif /* OTS_BASICGTULIFECYCLECONTROLLER_H_FM8ENVOH */

