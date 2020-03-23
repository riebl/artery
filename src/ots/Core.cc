#include "ots/Core.h"
#include "ots/GtuObject.h"
#include <omnetpp/cmessage.h>
#include <sim0mqpp/buffer_serialization.hpp>
#include <sim0mqpp/quantity.hpp>
#include <sim0mqpp/message.hpp>
#include <zmq.h>
#include <array>
#include <fstream>
#include <sstream>

namespace ots
{

Define_Module(Core)

namespace
{

const sim0mqpp::Identifier ready_msg = std::string("READY");
const sim0mqpp::Identifier time_changed_msg = std::string("TIME_CHANGED_EVENT");
const sim0mqpp::Identifier gtu_add_msg = std::string("NETWORK.GTU.ADD");
const sim0mqpp::Identifier gtu_remove_msg = std::string("NETWORK.GTU.REMOVE");
const sim0mqpp::Identifier gtu_position_msg = std::string("GTUPOSITION");

using namespace omnetpp;
const simsignal_t gtu_add_signal = cComponent::registerSignal("ots-gtu-add");
const simsignal_t gtu_remove_signal = cComponent::registerSignal("ots-gtu-remove");
const simsignal_t gtu_position_signal = cComponent::registerSignal("ots-gtu-position");

} // namespace


Core::Core()
{
    m_zmq_context = zmq_ctx_new();
    if (!m_zmq_context) {
        throw omnetpp::cRuntimeError("Creation of ZMQ context failed: %s", zmq_strerror(errno));
    }

    m_zmq_socket = zmq_socket(m_zmq_context, ZMQ_DEALER);
    if (!m_zmq_socket) {
        throw omnetpp::cRuntimeError("Creation of ZMQ socket failed: %s", zmq_strerror(errno));
    }

    m_step_event = new omnetpp::cMessage("OTS step");
}

Core::~Core()
{
    cancelAndDelete(m_step_event);

    if (m_zmq_socket) {
        zmq_close(m_zmq_socket);
    }

    if (m_zmq_context) {
        zmq_ctx_term(m_zmq_context);
    }
}

void Core::initialize()
{
    m_sim_federation = par("simFederation").stdstringValue();
    m_sim_sender = par("simSender").stdstringValue();
    m_sim_receiver = par("simReceiver").stdstringValue();

    // connect socket to OTS endpoint
    if (zmq_connect(m_zmq_socket, par("otsEndpoint").stringValue()) != 0) {
        throw omnetpp::cRuntimeError("Connecting to OTS endpoint failed: %s", zmq_strerror(errno));
    }

    // schedule first OTS step
    m_step_length = par("stepLength");
    scheduleAt(omnetpp::simTime(), m_step_event);
}

void Core::handleMessage(omnetpp::cMessage* msg)
{
    if (msg == m_step_event) {
        if (!m_network_loaded) {
            loadNetwork(par("otsNetworkFile").stringValue());
            m_network_loaded = true;
        }

        // trigger OTS to advance by step length
        simulateUntil(omnetpp::simTime());
        scheduleAt(omnetpp::simTime() + m_step_length, m_step_event);
        queryResponses(); // GTU add and remove events

        // request all GTU positions
        requestGtuPositions();
        queryResponses(); // GTU positions
    }
}

bool Core::receive(std::size_t maxlen, bool block)
{
    m_buffer.resize(maxlen);
    int length = zmq_recv(m_zmq_socket, m_buffer.data(), m_buffer.size(), block ? 0 : ZMQ_NOBLOCK);
    if (length < 0) {
        if (!block && errno == EAGAIN) {
            return false;
        } else {
            throw omnetpp::cRuntimeError("Receiving from OTS endpoint failed: %s", zmq_strerror(errno));
        }
    } else if (length > m_buffer.size()) {
        throw omnetpp::cRuntimeError("Length of replied message exceeds buffer size");
    } else {
        m_buffer.resize(length);
    }

    return true;
}

void Core::sendCommand(const std::string& cmd, std::vector<sim0mqpp::Any>&& payload)
{
    sim0mqpp::Message msg;
    msg.federation_id = m_sim_federation;
    msg.sender_id = m_sim_sender;
    msg.receiver_id = m_sim_receiver;
    msg.message_type_id = cmd;
    msg.message_id = 0;
    msg.payload = std::move(payload);

    sim0mqpp::Buffer buffer;
    sim0mqpp::BufferSerializer output(buffer);
    sim0mqpp::serialize(output, msg);
    if (!buffer.empty()) {
        if (zmq_send(m_zmq_socket, buffer.data(), buffer.size(), 0) < 0) {
            throw omnetpp::cRuntimeError("zmq_send failed: %s", zmq_strerror(errno));
        }
    } else {
        EV_ERROR << "Will not send empty buffer as sim0mqpp message\n";
    }
}

void Core::queryResponses()
{
    bool again = false;
    while (receive(8192, !again)) {
        sim0mqpp::BufferDeserializer input(m_buffer);
        sim0mqpp::Message msg;
        deserialize(input, msg);
        if (input.good()) {
            if (msg.message_type_id == ready_msg) {
                EV_TRACE << "READY received from OTS\n";
                again = true;
            } else if (msg.message_type_id == time_changed_msg) {
                // TODO add check if simulation times are in sync as soon as OTS reports time as scalar quantity
                EV_DETAIL << "time has changed in OTS\n";
            } else if (msg.message_type_id == gtu_add_msg) {
                processGtuAdd(msg);
            } else if (msg.message_type_id == gtu_remove_msg) {
                processGtuRemove(msg);
            } else if (msg.message_type_id == gtu_position_msg) {
                processGtuPosition(msg);
            } else {
                EV_WARN << "ignoring message " << sim0mqpp::to_string(msg.message_type_id) << "\n";
            }
        } else {
            throw omnetpp::cRuntimeError("Decoding sim0mqpp message failed: %s", input.error_message().c_str());
        }
    }
}

void Core::processGtuAdd(const sim0mqpp::Message& msg)
{
    const auto id = msg.get_payload<std::string>(0);
    if (id) {
        EV_TRACE << "add GTU " << *id << "\n";
        emit(gtu_add_signal, id->c_str());
    } else {
        EV_ERROR << "cannot read ID of GTU to be added\n";
    }
}

void Core::processGtuRemove(const sim0mqpp::Message& msg)
{
    auto id = msg.get_payload<std::string>(0);
    if (id) {
        EV_TRACE << "remove GTU " << *id << "\n";
        emit(gtu_remove_signal, id->c_str());
    } else {
        EV_ERROR << "cannot read ID of GTU to be removed\n";
    }
}

void Core::processGtuPosition(const sim0mqpp::Message& msg)
{
    auto id = msg.get_payload<std::string>(0);
    auto type = msg.get_payload<std::string>(1);
    auto x = msg.get_payload<double>(2);
    auto y = msg.get_payload<double>(3);
    auto z = msg.get_payload<double>(4);
    auto heading = msg.get_payload<double>(5);
    auto speed = msg.get_payload<sim0mqpp::Unit::Speed>(6);
    auto accel = msg.get_payload<sim0mqpp::Unit::Acceleration>(7);

    if (id && type && x && y && z && heading && speed && accel) {
        GtuObject gtu;
        gtu.setId(*id);
        gtu.setType(*type);
        gtu.setPosition({ *x, *y, *z});
        gtu.setHeadingRad(*heading);
        gtu.setSpeed(speed->value());
        gtu.setAcceleration(accel->value());
        emit(gtu_position_signal, &gtu);
        EV_TRACE << "GTU position update for ID " << *id << "\n";
    } else {
        EV_ERROR << "received broken GTU position\n";
    }
}

void Core::loadNetwork(const std::string& path)
{
    std::string ots_network;
    std::ifstream ifs(path);
    if (ifs) {
        // not really fast, but gets the job done for now
        std::ostringstream ss;
        ss << ifs.rdbuf();
        ots_network = ss.str();
    } else {
        throw omnetpp::cRuntimeError("Cannot read OTS network from %s", path);
    }

    omnetpp::SimTime ots_warmup = par("otsWarmupDuration");
    omnetpp::SimTime ots_run = par("otsRunDuration");
    std::int64_t ots_seed = par("otsSeed");

    std::vector<sim0mqpp::Any> payload;
    payload.push_back(ots_network);
    payload.push_back(sim0mqpp::ScalarQuantity<double> { ots_warmup.dbl(), sim0mqpp::Unit::Duration });
    payload.push_back(sim0mqpp::ScalarQuantity<double> { ots_run.dbl(), sim0mqpp::Unit::Duration });
    payload.push_back(ots_seed);

    sendCommand("LOADNETWORK", std::move(payload));
}

void Core::simulateUntil(omnetpp::SimTime time)
{
    std::vector<sim0mqpp::Any> payload;
    payload.push_back(sim0mqpp::ScalarQuantity<double> { time.dbl(), sim0mqpp::Unit::Time });
    sendCommand("SIMULATEUNTIL", std::move(payload));
}

void Core::requestGtuPositions()
{
    std::vector<sim0mqpp::Any> payload;
    sendCommand("SENDALLGTUPOSITIONS", std::move(payload));
}

} // namespace ots
