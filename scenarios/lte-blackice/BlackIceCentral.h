#ifndef BLACKICECENTRAL_H_3LKZ0NOB
#define BLACKICECENTRAL_H_3LKZ0NOB

#include <inet/networklayer/common/L3Address.h>
#include <inet/transportlayer/contract/udp/UdpSocket.h>
#include <omnetpp/csimplemodule.h>
#include <list>

// forward declaration
class BlackIceQuery;
class BlackIceReport;

class BlackIceCentral : public omnetpp::cSimpleModule, public inet::UdpSocket::ICallback
{
public:
    ~BlackIceCentral();

protected:
    void initialize() override;
    void finish() override;
    void handleMessage(omnetpp::cMessage*) override;

private:
    // inet::UdpSocket::ICallback interface
    void socketDataArrived(inet::UdpSocket*, inet::Packet*) override;
    void socketErrorArrived(inet::UdpSocket*, inet::Indication*) override;

    void processReport(const BlackIceReport&);
    void processQuery(const BlackIceQuery&, const inet::L3Address&, int port);
    void disseminateWarning();

    int reportPort;
    int queryPort;
    int numReceivedWarnings;
    int numReceivedQueries;
    inet::UdpSocket reportSocket;
    inet::UdpSocket querySocket;
    std::list<BlackIceReport> reports;
};

#endif /* BLACKICECENTRAL_H_3LKZ0NOB */

