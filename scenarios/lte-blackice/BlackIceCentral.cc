#include "BlackIceCentral.h"
#include "lte_msgs/BlackIceWarning_m.h"
#include <inet/networklayer/common/L3AddressResolver.h>
#include <inet/transportlayer/contract/udp/UDPControlInfo.h>

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

    queryPort = par("queryPort");
    querySocket.setOutputGate(gate("udpOut"));
    querySocket.bind(inet::L3Address(), queryPort);

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
    if (msg->getKind() == inet::UDP_I_DATA) {
        processPacket(PK(msg));
    } else if (msg->getKind() == inet::UDP_I_ERROR) {
        EV_ERROR << "UDP error occurred\n";
        delete msg;
    } else {
        throw cRuntimeError("Unrecognized message (%s)%s", msg->getClassName(), msg->getName());
    }
}

void BlackIceCentral::processPacket(cPacket* pkt)
{
    auto ctrl = pkt->getControlInfo();
    if (auto udp = dynamic_cast<inet::UDPDataIndication*>(ctrl)) {
        if (udp->getDestPort() == reportPort) {
            processReport(*check_and_cast<BlackIceReport*>(pkt));
        } else if (udp->getDestPort() == queryPort) {
            processQuery(*check_and_cast<BlackIceQuery*>(pkt), udp->getSrcAddr(), udp->getSrcPort());
        } else {
            throw cRuntimeError("Unknown UDP destination port %d", udp->getDestPort());
        }
    }
    delete pkt;
}

void BlackIceCentral::processReport(BlackIceReport& report)
{
    ++numReceivedWarnings;
    reports.push_back(report);
}

void BlackIceCentral::processQuery(BlackIceQuery& query, const inet::L3Address& addr, int port)
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
    querySocket.sendTo(response, addr, port);
}

