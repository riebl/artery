#ifndef BLACKICECENTRAL_H_3LKZ0NOB
#define BLACKICECENTRAL_H_3LKZ0NOB

#include <inet/networklayer/common/L3Address.h>
#include <inet/transportlayer/contract/udp/UDPSocket.h>
#include <omnetpp/csimplemodule.h>
#include <list>

// forward declaration
class BlackIceQuery;
class BlackIceReport;

class BlackIceCentral : public omnetpp::cSimpleModule
{
public:
    ~BlackIceCentral();

protected:
    void initialize() override;
    void finish() override;
    void handleMessage(omnetpp::cMessage*) override;

private:
    void processPacket(omnetpp::cPacket*);
    void processReport(BlackIceReport&);
    void processQuery(BlackIceQuery&, const inet::L3Address&, int port);
    void disseminateWarning();

    int reportPort;
    int queryPort;
    int numReceivedWarnings;
    int numReceivedQueries;
    inet::UDPSocket reportSocket;
    inet::UDPSocket querySocket;
    std::list<BlackIceReport> reports;
};

#endif /* BLACKICECENTRAL_H_3LKZ0NOB */

