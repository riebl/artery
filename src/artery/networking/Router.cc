#include "artery/application/Middleware.h"
#include "artery/networking/GeoNetIndication.h"
#include "artery/networking/GeoNetPacket.h"
#include "artery/networking/IDccEntity.h"
#include "artery/networking/PositionFixObject.h"
#include "artery/networking/Router.h"
#include "artery/networking/Runtime.h"
#include "artery/networking/SecurityEntity.h"
#include "artery/nic/RadioDriverBase.h"
#include "artery/nic/RadioDriverProperties.h"
#include "artery/utility/InitStages.h"
#include "artery/utility/PointerCheck.h"
#include <inet/common/ModuleAccess.h>
#include <vanetza/btp/header.hpp>
#include <vanetza/btp/header_conversion.hpp>
#include <vanetza/geonet/data_confirm.hpp>

namespace artery
{

Define_Module(Router)

static const omnetpp::simsignal_t scPositionFixSignal = omnetpp::cComponent::registerSignal("PositionFix");

int Router::numInitStages() const
{
    return InitStages::Total;
}

void Router::initialize(int stage)
{
    if (stage == InitStages::Prepare) {
        getParentModule()->subscribe(scPositionFixSignal, this);
        mMiddleware = inet::getModuleFromPar<Middleware>(par("middlewareModule"), this);
        mRadioDriver = inet::getModuleFromPar<RadioDriverBase>(par("radioDriverModule"), this);
        mRadioDriverDataIn = gate("radioDriverData");
        mRadioDriverPropertiesIn = gate("radioDriverProperties");
        mSecurityEntity = inet::findModuleFromPar<SecurityEntity>(par("securityModule"), this, false);
    } else if (stage == InitStages::Self) {
        // initialize MIB (will check for existence of security entity)
        initializeManagementInformationBase(mMIB);

        // basic router setup
        auto runtime = inet::getModuleFromPar<Runtime>(par("runtimeModule"), this);
        mRouter.reset(new vanetza::geonet::Router(*runtime, mMIB));
        vanetza::MacAddress init_mac = vanetza::create_mac_address(getId());
        mRouter->set_address(generateAddress(init_mac));

        // register security entity if available
        if (mSecurityEntity) {
            mRouter->set_security_entity(mSecurityEntity);
        }

        // bind router to DCC entity
        auto dccEntity = inet::findModuleFromPar<IDccEntity>(par("dccModule"), this);
        mRouter->set_access_interface(notNullPtr(dccEntity->getRequestInterface()));
        mRouter->set_dcc_field_generator(dccEntity->getGeonetFieldGenerator()); // nullptr is okay

        // pass BTP-B messages to transport layer dispatcher in network interface
        using vanetza::geonet::UpperProtocol;
        mNetworkInterface = std::make_shared<NetworkInterface>(*this, *dccEntity, mMiddleware->getTransportDispatcher());
        mRouter->set_transport_handler(UpperProtocol::BTP_B, &mNetworkInterface->getTransportHandler());

        // finally, register new network interface at middleware
        mMiddleware->registerNetworkInterface(mNetworkInterface);
    }
}

void Router::finish()
{
    mRouter.reset();
}

void Router::receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t signal, omnetpp::cObject* obj, omnetpp::cObject*)
{
    if (signal == scPositionFixSignal) {
        auto fix = dynamic_cast<PositionFixObject*>(obj);
        if (fix && mRouter) {
            mRouter->update_position(*fix);
        }
    }
}

void Router::handleMessage(omnetpp::cMessage* msg)
{
    if (msg->getArrivalGate() == mRadioDriverDataIn) {
        auto* packet = omnetpp::check_and_cast<GeoNetPacket*>(msg);
        auto* indication = omnetpp::check_and_cast<GeoNetIndication*>(packet->getControlInfo());
        mRouter->indicate(std::move(*packet).extractPayload(), indication->source, indication->destination);
    } else if (msg->getArrivalGate() == mRadioDriverPropertiesIn) {
        auto* properties = omnetpp::check_and_cast<RadioDriverProperties*>(msg);
        auto addr = generateAddress(properties->LinkLayerAddress);
        mRouter->set_address(addr);
        Identity identity;
        identity.geonet.insert({mNetworkInterface, addr});
        emit(Identity::changeSignal, Identity::ChangeGeoNetAddress, &identity);
        mNetworkInterface->channel = properties->ServingChannel;
    } else {
        error("Do not know how to handle received message");
    }

    delete msg;
}

void Router::initializeManagementInformationBase(vanetza::geonet::ManagementInformationBase& mib)
{
    mib.itsGnDefaultTrafficClass.tc_id(3); // send BEACONs with DP3
    mib.itsGnIsMobile = par("isMobile");
    mib.itsGnSecurity = (mSecurityEntity != nullptr);
    mib.vanetzaDeferInitialBeacon = par("deferInitialBeacon");
}

void Router::request(const vanetza::btp::DataRequestB& request, std::unique_ptr<vanetza::DownPacket> packet)
{
    ASSERT(mRouter);
    Enter_Method("request");

    using namespace vanetza;
    btp::HeaderB btp_header;
    btp_header.destination_port = request.destination_port;
    btp_header.destination_port_info = request.destination_port_info;
    packet->layer(OsiLayer::Transport) = btp_header;

    geonet::DataConfirm confirm;
    if (request.gn.transport_type == geonet::TransportType::SHB) {
        geonet::ShbDataRequest shb(mMIB);
        copy_request_parameters(request, shb);
        confirm = mRouter->request(shb, std::move(packet));
    } else if (request.gn.transport_type == geonet::TransportType::GBC) {
        geonet::GbcDataRequest gbc(mMIB);
        copy_request_parameters(request, gbc);
        confirm = mRouter->request(gbc, std::move(packet));
    } else {
        error("Unknown or unimplemented transport type");
    }

    if (confirm.rejected()) {
        error("GN-Data.request rejected");
    }
}

const vanetza::geonet::LocationTable& Router::getLocationTable() const
{
    if (!mRouter) {
        error("Router::getLocationTable called before initialization");
    }
    return mRouter->get_location_table();
}

vanetza::geonet::Address Router::getAddress() const
{
    if (!mRouter) {
        error("Router::getAddress called before initialization");
    }
    return mRouter->get_local_position_vector().gn_addr;
}

vanetza::geonet::Address Router::generateAddress(const vanetza::MacAddress& mac)
{
    vanetza::geonet::Address gnAddr;
    gnAddr.is_manually_configured(true);
    // VehicleMiddleware determines station type during initialisation
    gnAddr.station_type(mMiddleware->getStationType());
    gnAddr.country_code(0);
    gnAddr.mid(mac);
    return gnAddr;
}

} // namespace artery
