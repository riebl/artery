#include "ots/BasicGtuLifecycleController.h"
#include <omnetpp/ccomponenttype.h>
#include <omnetpp/cmessage.h>

namespace ots
{

Define_Module(BasicGtuLifecycleController)

namespace
{

using namespace omnetpp;
const simsignal_t otsLifecycleSignal = cComponent::registerSignal("ots-lifecycle");
const simsignal_t otsGtuAddSignal = cComponent::registerSignal("ots-gtu-add");
const simsignal_t otsGtuRemoveSignal = cComponent::registerSignal("ots-gtu-remove");
const simsignal_t otsGtuPositionSignal = cComponent::registerSignal("ots-gtu-position");

class InsertionMessage : public omnetpp::cMessage
{
public:
    InsertionMessage(const std::string& gtu_id) :
        omnetpp::cMessage("GTU insertion"), mGtuId(gtu_id) {}
    const std::string& getGtuId() const { return mGtuId; }
    const char* getDisplayString() const override { return mGtuId.c_str(); }
    omnetpp::cMessage* dup() const override { return new InsertionMessage(*this); }

private:
    std::string mGtuId;
};

} // namespace

void BasicGtuLifecycleController::initialize()
{
    if (auto emitter = getParentModule()) {
        emitter->subscribe(otsLifecycleSignal, this);
        emitter->subscribe(otsGtuAddSignal, this);
        emitter->subscribe(otsGtuRemoveSignal, this);
        emitter->subscribe(otsGtuPositionSignal, this);
    }

    m_creation_policy = dynamic_cast<GtuCreationPolicy*>(getSubmodule("creationPolicy"));
    if (!m_creation_policy) {
        throw cRuntimeError("missing GtuCreationPolicy");
    }
}

void BasicGtuLifecycleController::handleMessage(omnetpp::cMessage* msg)
{
    if (auto insertion_msg = dynamic_cast<InsertionMessage*>(msg)) {
        auto found = m_pending_gtus.find(insertion_msg->getGtuId());
        if (found != m_pending_gtus.end()) {
            ASSERT(found->second);
            createSink(*found->second);
            m_pending_gtus.erase(found);
        }
    }
    delete msg;
}

void BasicGtuLifecycleController::receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t signal, bool flag, omnetpp::cObject*)
{
    if (signal == otsLifecycleSignal && !flag) {
        m_pending_gtus.clear();
        m_gtu_sinks.clear();
        while (!m_nodes.empty()) {
            removeModule(m_nodes.begin()->first);
        }
    }
}

void BasicGtuLifecycleController::receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t signal, const char* id, omnetpp::cObject*)
{
    if (signal == otsGtuAddSignal) {
        addGtu(id);
    } else if (signal == otsGtuRemoveSignal) {
        removeGtu(id);
    } else {
        EV_WARN << "ignoring unknown signal\n";
    }
}

void BasicGtuLifecycleController::receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t signal, omnetpp::cObject* obj, omnetpp::cObject*)
{
    if (signal == otsGtuPositionSignal) {
        auto gtu = dynamic_cast<GtuObject*>(obj);
        if (gtu) {
            updateGtu(*gtu);
        }
    } else {
        EV_WARN << "ignoring unknown signal\n";
    }
}

void BasicGtuLifecycleController::addGtu(const std::string& id)
{
    m_pending_gtus.insert(std::make_pair(id, nullptr));
}

void BasicGtuLifecycleController::removeGtu(const std::string& id)
{
    removeModule(id);
    m_pending_gtus.erase(id);
    m_gtu_sinks.erase(id);
}

void BasicGtuLifecycleController::updateGtu(const GtuObject& obj)
{
    Enter_Method_Silent();
    GtuSink* sink = getSink(obj.getId());
    if (!sink) {
        auto pending = m_pending_gtus.find(obj.getId());
        if (pending != m_pending_gtus.end()) {
            if (pending->second == nullptr) {
                omnetpp::SimTime delay = par("insertionDelay");
                if (delay > omnetpp::SimTime::ZERO) {
                    // schedule insertion later on
                    pending->second.reset(new GtuObject { obj });
                    scheduleAt(simTime() + delay, new InsertionMessage(obj.getId()));
                } else {
                    // insert immediately
                    m_pending_gtus.erase(obj.getId());
                    createSink(obj);
                }
            } else {
                // update GTU object used for later insertion
                *pending->second = obj;
            }
        } else {
            EV_WARN << "no GTU sink found for id " << obj.getId() << "\n";
        }
    } else {
        sink->update(obj);
    }
}

omnetpp::cModule* BasicGtuLifecycleController::addModule(const std::string& id, omnetpp::cModuleType* type, Initializer& init)
{
    omnetpp::cModule* mod = type->create("gtu", getSystemModule(), m_node_index, m_node_index);
    ++m_node_index;
    mod->finalizeParameters();
    mod->buildInside();
    m_nodes[id] = mod;
    init(mod);
    mod->scheduleStart(simTime());
    mod->callInitialize();
    return mod;
}

void BasicGtuLifecycleController::removeModule(const std::string& id)
{
    omnetpp::cModule* mod = getModule(id);
    if (mod) {
        mod->callFinish();
        mod->deleteModule();
        m_nodes.erase(id);
    } else {
        EV_ERROR << "cannot remove non-existing GTU module for id " << id << "\n";
    }
}

omnetpp::cModule* BasicGtuLifecycleController::getModule(const std::string& id)
{
    auto found = m_nodes.find(id);
    return found != m_nodes.end() ? found->second : nullptr;
}

GtuSink* BasicGtuLifecycleController::getSink(const std::string& id)
{
    auto found = m_gtu_sinks.find(id);
    return found != m_gtu_sinks.end() ? found->second : nullptr;
}

void BasicGtuLifecycleController::createSink(const GtuObject& obj)
{
    GtuSink* sink = nullptr;
    const GtuCreationPolicy::Instruction& instruction = m_creation_policy->getInstruction(obj);
    Initializer init = [&sink, &instruction, &obj](omnetpp::cModule* mod) {
        sink = dynamic_cast<GtuSink*>(mod->getModuleByPath(instruction.getSinkPath()));
        if (sink) {
            sink->initialize(obj);
        }
    };
    omnetpp::cModule* mod = addModule(obj.getId(), instruction.getModuleType(), init);

    if (sink) {
        auto insertion = m_gtu_sinks.insert({ obj.getId(), sink });
        if (!insertion.second) {
            throw omnetpp::cRuntimeError("insertion of GTU sink failed");
        }
    } else {
        EV_ERROR << "could not find GTU sink for module " << mod->getFullPath() << "\n";
    }
}

} // namespace ots
