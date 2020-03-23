#ifndef OTS_CORE_QMT3LWQG
#define OTS_CORE_QMT3LWQG

#include <omnetpp/csimplemodule.h>
#include <sim0mqpp/any.hpp>
#include <sim0mqpp/message.hpp>
#include <string>
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
    void sendCommand(const std::string& cmd, std::vector<sim0mqpp::Any>&& payload);
    void loadNetwork(const std::string& path);
    void simulateUntil(omnetpp::SimTime);
    void requestGtuPositions();

    bool receive(std::size_t maxlen, bool block = true);
    void queryResponses();
    void processGtuPosition(const sim0mqpp::Message&);
    void processGtuAdd(const sim0mqpp::Message&);
    void processGtuRemove(const sim0mqpp::Message&);

private:
    void* m_zmq_context = nullptr;
    void* m_zmq_socket = nullptr;
    bool m_network_loaded = false;
    omnetpp::cMessage* m_step_event = nullptr;
    omnetpp::SimTime m_step_length;
    std::string m_sim_federation;
    std::string m_sim_sender;
    std::string m_sim_receiver;
    std::vector<std::uint8_t> m_buffer;
};

} // namespace ots

#endif /* OTS_CORE_QMT3LWQG */

