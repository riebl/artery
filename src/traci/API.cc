#include "traci/API.h"
#include "traci/Launcher.h"
#include <thread>

namespace traci
{

API::Version API::getVersion() const
{
    using constants::CMD_GETVERSION;

    tcpip::Storage outMsg;
    outMsg.writeUnsignedByte(1 + 1);
    outMsg.writeUnsignedByte(CMD_GETVERSION);
    mySocket->sendExact(outMsg);

    tcpip::Storage inMsg;
    check_resultState(inMsg, CMD_GETVERSION);
    int cmdId = check_commandGetResult(inMsg, 0, -1, true);
    if (cmdId != constants::CMD_GETVERSION) {
        throw tcpip::SocketException("#Error: rceived status response to command: " + toString(cmdId) + " but expected: " + toString(CMD_GETVERSION));
    }

    Version v;
    v.first = inMsg.readInt();
    v.second = inMsg.readString();
    return v;
}

TraCIGeoPosition API::convertGeo(const TraCIPosition& pos) const
{
    using namespace constants;

    tcpip::Storage addParams;
    addParams.writeUnsignedByte(TYPE_COMPOUND);
    addParams.writeInt(2);
    addParams.writeUnsignedByte(POSITION_2D);
    addParams.writeDouble(pos.x);
    addParams.writeDouble(pos.y);
    addParams.writeUnsignedByte(TYPE_UBYTE);
    addParams.writeUnsignedByte(POSITION_LON_LAT);
    send_commandGetVariable(CMD_GET_SIM_VARIABLE, POSITION_CONVERSION, "", &addParams);

    tcpip::Storage inMsg;
    processGET(inMsg, CMD_GET_SIM_VARIABLE, POSITION_LON_LAT);
    TraCIGeoPosition geo;
    geo.longitude = inMsg.readDouble();
    geo.latitude = inMsg.readDouble();
    return geo;
}

void API::connect(const ServerEndpoint& endpoint)
{
    const unsigned max_tries = endpoint.retry ? 10 : 0;
    unsigned tries = 0;
    auto sleep = std::chrono::milliseconds(500);

    while (true) {
        try {
            TraCIAPI::connect(endpoint.hostname, endpoint.port);
            return;
        } catch (tcpip::SocketException&) {
            if (++tries < max_tries) {
                std::this_thread::sleep_for(sleep);
                sleep *= 2;
            } else {
                throw;
            }
        }
    }
}

} // namespace traci
