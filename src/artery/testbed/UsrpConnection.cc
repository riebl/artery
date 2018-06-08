#include "artery/testbed/UsrpConnection.h"
#include "artery/testbed/OtaInterfaceUsrp.h"
#include <iostream>

namespace artery
{

UsrpConnection::UsrpConnection(const SeaConnectionParams& params, OtaInterfaceUsrp* otaInterfaceCallback) :
    mV2X(new sea_v2x::V2X(params.listeningPort)), // this machine's listening port
    mConnectionParams(params), mOtaInterfaceCallback(otaInterfaceCallback)
{
    mV2X->set_HCM_address(params.hcmIP.c_str());    // HCM IP
    mV2X->set_destination_port(params.hcmPort);     // HCM Listenig Port
    setHandlerFunctions(*mV2X);
    mV2X->start();

    connectToUsrp();
}

UsrpConnection::~UsrpConnection()
{
    if (mV2X) {
        mV2X->stop();
        mV2X.release();
    }
}

void UsrpConnection::connectToUsrp()
{
    // get arbitrary (e.g. AGC_Enable) to exercise GET_indication handler
    sea_v2x::PARAMETER_GET get;
    get.type = sea_v2x::ParameterType::Frequency;
    mV2X->send_PARAMETER_GET(get);

    for (int c = 0; mConnectionParams.connectTimeout > c; ++c)
    {
        std::cout << "Try to connect to USRP" << std::endl;
        std::unique_lock<std::mutex> lock(mMutex);
        if (mConnectionOk) { /*< modified by GET_indication handler */
            std::cout << "Connection to USRP established" << std::endl;
            return;
        }
        lock.unlock();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    throw omnetpp::cRuntimeError("Unable to connect to USRP");
}

void UsrpConnection::setHandlerFunctions(sea_v2x::V2X& v2x)
{
    v2x.set_handle_PARAMETER_SET([](sea_v2x::PARAMETER_SET set) {
        //std::cout << "Parameter Set!" << std::endl;
    });

    v2x.set_handle_PARAMETER_SET_indication([](sea_v2x::PARAMETER_SET_indication set) {
        //EV_INFO << "Parameter Set Indication!" << std::endl;
    });

    v2x.set_handle_PARAMETER_GET([](sea_v2x::PARAMETER_GET get) {
        //std::cout << "Parameter Get!" << std::endl;
    });

    v2x.set_handle_PARAMETER_GET_indication([this](sea_v2x::PARAMETER_GET_indication get) {
        //EV_INFO << "Parameter Get Indication!" << std::endl;
        std::lock_guard<std::mutex> lock(mMutex);
        mConnectionOk = true;
    });

    v2x.set_handle_MA_UNITDATA_request([](sea_v2x::MA_UNITDATA_request r) {
        std::cout << "Unitdata Request!" << std::endl;
    });

    // Notifies the OtaIndicationQueue to generate a thread safe sending event
    v2x.set_handle_MA_UNITDATA_indication([this](sea_v2x::MA_UNITDATA_indication i) {
        mOtaInterfaceCallback->notifyQueue(i);
    });
}

void UsrpConnection::configureUsrp()
{
    sea_v2x::PARAMETER_SET set;
    // set.type = sea_v2x::ParameterType::Frequency;
    // double f = 5.9e9;
    // uint8_t* v = reinterpret_cast<uint8_t*>(&f);
    // set.value = { v[7], v[6], v[5], v[4], v[3], v[2], v[1], v[0] };
    // mV2X->send_PARAMETER_SET(set);

    set.type = sea_v2x::ParameterType::AGC_Enable;
    set.value = {0};
    mV2X->send_PARAMETER_SET(set);
}

void UsrpConnection::sendPacket(const vanetza::MacAddress& source, const vanetza::MacAddress& destination, const vanetza::byte_view_range& data)
{
    // Routing information should be set to 0 according to IEEE 802.11 section 5.2.2 MA-UNITDATA.request
    static const uint8_t Ieee80211RoutingInformation = 0;
    // According to ETSI ES 202 663 V1.1.0 section 5.3
    static const uint8_t EtsiModulationCodingScheme = 2;
    // Highest priority according to IEEE 802.11 section 5.1.1.4 and ETSI EN 302 663 V1.2.1 Table B.3
    static const uint8_t IeeeEtsiServiceClass = 7;

    sea_v2x::MA_UNITDATA_request request;
    request.data = {data.begin(), data.end()};
    request.destination_address = destination.octets;
    request.source_address = source.octets;
    request.routing_information = Ieee80211RoutingInformation;
    request.priority = IeeeEtsiServiceClass;
    request.service_class = sea_v2x::ServiceClassType::AC_VO; // transmission with fastest channel access
    request.tx_power_level = 23; // High enough transmission power to make Cohda MK5 receive the packet
    request.mcs = EtsiModulationCodingScheme;

    mV2X->send_MA_UNITDATA_request(request);
}

void UsrpConnection::shutDownConnection()
{
    mV2X->stop();
    mV2X.release();
}

} // namespace artery
