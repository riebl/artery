#include "traci/API.h"
#include "traci/Launcher.h"
#include <thread>

namespace traci
{

API::Version API::getVersion() const
{
    using libsumo::CMD_GETVERSION;

    tcpip::Storage outMsg;
    outMsg.writeUnsignedByte(1 + 1);
    outMsg.writeUnsignedByte(CMD_GETVERSION);
    mySocket->sendExact(outMsg);

    tcpip::Storage inMsg;
    check_resultState(inMsg, CMD_GETVERSION);
    int cmdId = check_commandGetResult(inMsg, 0, -1, true);
    if (cmdId != CMD_GETVERSION) {
        throw tcpip::SocketException("#Error: rceived status response to command: " + toString(cmdId) + " but expected: " + toString(CMD_GETVERSION));
    }

    Version v;
    v.first = inMsg.readInt();
    v.second = inMsg.readString();
    return v;
}

TraCIGeoPosition API::convertGeo(const TraCIPosition& pos) const
{
    libsumo::TraCIPosition result = simulation.convertGeo(pos.x, pos.y, false);
    TraCIGeoPosition geo;
    geo.longitude = result.x;
    geo.latitude = result.y;
    return geo;
}

TraCIPosition API::convert2D(const TraCIGeoPosition& pos) const
{
    return simulation.convertGeo(pos.longitude, pos.latitude, true);
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
