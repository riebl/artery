#include "artery/ots/GtuProxyService.h"
#include "ots/Core.h"
#include "ots/GtuObject.h"
#include "ots/RadioMessage.h"
#include <inet/common/ModuleAccess.h>
#include <vanetza/btp/data_request.hpp>
#include <vanetza/dcc/profile.hpp>
#include <vanetza/geonet/interface.hpp>

namespace artery
{

Register_Class(GtuProxyService)

GtuProxyService::~GtuProxyService()
{
    mCore->unregisterRadio(this);
}

void GtuProxyService::initialize()
{
    ItsG5Service::initialize();
    mItsAid = par("aid");

    auto& gtu = getFacilities().getConst<ots::GtuObject>();
    mGtuId = gtu.getId();

    mCore = inet::getModuleFromPar<ots::Core>(par("otsCore"), this);
    mCore->registerRadio(mGtuId, this);
}

void GtuProxyService::indicate(const vanetza::btp::DataIndication& indication, omnetpp::cPacket* packet)
{
    Enter_Method("indicate");
    auto msg = dynamic_cast<ots::RadioMessage*>(packet);
    if (msg) {
        if (msg->getReceiver() == mGtuId) {
            EV_DETAIL << "notify OTS about received radio packet\n";
            mCore->notifyRadioReception(*msg);
        } else {
            EV_DETAIL << "ignore packet addressed to another GTU\n";
        }
    } else {
        EV_ERROR << "received packet is not a GTU radio message\n";
    }
    delete packet;
}

void GtuProxyService::onRadioTransmit(std::unique_ptr<ots::RadioMessage> msg)
{
    Enter_Method("onRadioTransmit");
    using namespace vanetza;

    auto& mco = getFacilities().get_const<MultiChannelPolicy>();
    auto& networks = getFacilities().get_const<NetworkInterfaceTable>();
    auto channels = mco.allChannels(mItsAid);
    for (int i = channels.size() - 1; i >= 0; --i) {
        auto channel = channels[i];
        auto network = networks.select(channel);
        if (network) {
            btp::DataRequestB req;
            req.destination_port = host_cast(getPortNumber(channel));
            req.gn.transport_type = geonet::TransportType::SHB;
            req.gn.traffic_class.tc_id(static_cast<unsigned>(dcc::Profile::DP2));
            req.gn.communication_profile = geonet::CommunicationProfile::ITS_G5;
            req.gn.its_aid = mItsAid;
            request(req, (i == 0 ? msg.release() : msg->dup()), network.get());
            EV_DETAIL << "transmit OTS radio message on channel " << channel << "\n";
        } else {
            EV_ERROR << "No network interface available for channel " << channel << "\n";
        }
    }
}

} // namespace artery
