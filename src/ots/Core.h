#ifndef OTS_CORE_QMT3LWQG
#define OTS_CORE_QMT3LWQG

#include <omnetpp/csimplemodule.h>
#include <sim0mqpp/any.hpp>
#include <sim0mqpp/message.hpp>
#include <string>
#include <unordered_set>
#include <vector>

namespace ots
{

class Core : public omnetpp::cSimpleModule
{
public:
    Core();
    virtual ~Core();

    void initialize() override;
    void handleMessage(omnetpp::cMessage*) override;

protected:
    void sendCommand(const sim0mqpp::Identifier& cmd);
    void sendCommand(const sim0mqpp::Identifier& cmd, std::int32_t cmd_id);
    void sendCommand(const sim0mqpp::Identifier& cmd, std::int32_t cmd_id, std::vector<sim0mqpp::Any>&& payload);
    void sendCommand(const sim0mqpp::Identifier& cmd, std::vector<sim0mqpp::Any>&& payload);
    void startSimulation(const std::string& path);
    void stopSimulation();
    void simulateUntil(omnetpp::SimTime);
    void requestGtuPositions();
    void requestGtuPosition(const sim0mqpp::Any&);

    bool receive(std::size_t maxlen, bool block = true);
    void queryResponses(const sim0mqpp::Identifier&);
    void processNetwork(const sim0mqpp::Message&);
    void processGtuMove(const sim0mqpp::Message&);
    void processGtuAdd(const sim0mqpp::Message&);
    void processGtuRemove(const sim0mqpp::Message&);
    void processSimulationStart(const sim0mqpp::Message&);
    void processSimulationTrigger(const sim0mqpp::Message&);
    void processSimulationChange(const sim0mqpp::Message&);
    void processSubscriptionReply(const sim0mqpp::Message&, const std::string&);

private:
    void* m_zmq_context = nullptr;
    void* m_zmq_socket = nullptr;
    bool m_network_loaded = false;
    omnetpp::cMessage* m_step_event = nullptr;
    omnetpp::SimTime m_step_length;
    omnetpp::SimTime m_stop_time;
    omnetpp::SimTime m_ots_time;
    std::string m_sim_federation;
    std::string m_sim_sender;
    std::string m_sim_receiver;
    std::vector<std::uint8_t> m_buffer;
    std::unordered_set<sim0mqpp::Identifier> m_pending;
    bool m_gtu_add_subscribed = false;
    bool m_gtu_remove_subscribed = false;
    bool m_sim_state_subscribed = false;
};

} // namespace ots

#endif /* OTS_CORE_QMT3LWQG */

