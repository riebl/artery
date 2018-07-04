#include "BlackIceCentral.h"
#include "lte_msgs/BlackIceWarning_m.h"
#include <inet/common/packet/chunk/cPacketChunk.h>
#include <inet/networklayer/common/L3AddressResolver.h>
#include <inet/networklayer/common/L3AddressTag_m.h>
#include <inet/transportlayer/common/L4PortTag_m.h>

using namespace omnetpp;

Define_Module(BlackIceCentral)

BlackIceCentral::~BlackIceCentral()
{
}

void BlackIceCentral::initialize()
{
    reportPort = par("reportPort");
    reportSocket.setOutputGate(gate("udpOut"));
    reportSocket.bind(inet::L3Address(), reportPort);
    reportSocket.setCallback(this);

    queryPort = par("queryPort");
    querySocket.setOutputGate(gate("udpOut"));
    querySocket.bind(inet::L3Address(), queryPort);
    querySocket.setCallback(this);

    numReceivedWarnings = 0;
    numReceivedQueries = 0;
    WATCH(numReceivedWarnings);
    WATCH(numReceivedQueries);
}

void BlackIceCentral::finish()
{
    reportSocket.close();
    querySocket.close();

    recordScalar("numReceivedWarnings", numReceivedWarnings);
    recordScalar("numReceivedQueries", numReceivedQueries);
}

void BlackIceCentral::handleMessage(cMessage* msg)
{
    if (reportSocket.belongsToSocket(msg)) {
        reportSocket.processMessage(msg);
    } else if (querySocket.belongsToSocket(msg)) {
        querySocket.processMessage(msg);
    } else {
        error("Unrecognized message (%s)%s", msg->getClassName(), msg->getName());
        delete msg;
    }
}

void BlackIceCentral::socketDataArrived(inet::UdpSocket* socket, inet::Packet* packet)
{
    if (socket == &reportSocket) {
        auto chunk = packet->popAtFront<inet::cPacketChunk>();
        processReport(*omnetpp::check_and_cast<BlackIceReport*>(chunk->getPacket()));
    } else if (socket == &querySocket) {
        inet::L3Address srcAddr = packet->getTag<inet::L3AddressInd>()->getSrcAddress();
        int srcPort = packet->getTag<inet::L4PortInd>()->getSrcPort();
        auto chunk = packet->popAtFront<inet::cPacketChunk>();
        processQuery(*omnetpp::check_and_cast<BlackIceQuery*>(chunk->getPacket()), srcAddr, srcPort);
    }
    delete packet;
}

void BlackIceCentral::socketErrorArrived(inet::UdpSocket* socket, inet::Indication* indication)
{
    EV_ERROR << "UDP error occurred\n";
    delete indication;
}

void BlackIceCentral::processReport(const BlackIceReport& report)
{
    ++numReceivedWarnings;
    reports.push_back(report);
}

void BlackIceCentral::processQuery(const BlackIceQuery& query, const inet::L3Address& addr, int port)
{
    ++numReceivedQueries;
    int warnings = 0;
    for (auto& report : reports) {
        double dx = query.getPositionX() - report.getPositionX();
        double dy = query.getPositionY() - report.getPositionY();
        if (dx * dx + dy * dy < query.getRadius() * query.getRadius()) {
            ++warnings;
        }
    }

    auto response = new BlackIceResponse("black ice response");
    response->setWarnings(warnings);
    auto packet = new inet::Packet(response->getName());
    packet->insertAtFront(inet::makeShared<inet::cPacketChunk>(response));
    querySocket.sendTo(packet, addr, port);
}

