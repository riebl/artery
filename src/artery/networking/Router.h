#ifndef ARTERY_ROUTER_H_1YTFC6NB
#define ARTERY_ROUTER_H_1YTFC6NB

#include <omnetpp/csimplemodule.h>
#include <vanetza/geonet/mib.hpp>
#include <vanetza/geonet/router.hpp>
#include <vanetza/btp/data_request.hpp>
#include <vanetza/security/security_entity.hpp>
#include <memory>

namespace artery
{

class Middleware;
class NetworkInterface;
class RadioDriverBase;

class Router : public omnetpp::cSimpleModule, public omnetpp::cListener
{
    public:
        // cSimpleModule
        int numInitStages() const override;
        void initialize(int stage) override;
        void finish() override;
        void handleMessage(omnetpp::cMessage*) override;

        // cListener
        void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t, omnetpp::cObject*, omnetpp::cObject*) override;

        void request(const vanetza::btp::DataRequestB&, std::unique_ptr<vanetza::DownPacket>);
        vanetza::geonet::Address getAddress() const;
        const vanetza::geonet::LocationTable& getLocationTable() const;

    protected:
        virtual void initializeManagementInformationBase(vanetza::geonet::ManagementInformationBase&);
        vanetza::geonet::Address generateAddress(const vanetza::MacAddress&);

    private:
        vanetza::geonet::ManagementInformationBase mMIB;
        std::unique_ptr<vanetza::geonet::Router> mRouter;
        Middleware* mMiddleware = nullptr;
        vanetza::security::SecurityEntity* mSecurityEntity = nullptr;
        RadioDriverBase* mRadioDriver;
        omnetpp::cGate* mRadioDriverDataIn;
        omnetpp::cGate* mRadioDriverPropertiesIn;
        std::shared_ptr<NetworkInterface> mNetworkInterface;
};

} // namespace artery

#endif /* ARTERY_ROUTER_H_1YTFC6NB */

