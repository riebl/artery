#include "artery/networking/AccessInterface.h"
#include "artery/networking/GeoNetPacket.h"
#include "artery/networking/GeoNetRequest.h"
#include "artery/utility/PointerCheck.h"
#include <omnetpp/checkandcast.h>
#include <omnetpp/csimplemodule.h>
#include <omnetpp/simutil.h>

using vanetza::access::DataRequest;
using vanetza::ChunkPacket;

namespace artery
{

AccessInterface::AccessInterface(omnetpp::cGate* gate) :
    mGateOut(notNullPtr(gate)),
    mModuleOut(omnetpp::check_and_cast<omnetpp::cSimpleModule*>(mGateOut->getOwnerModule()))
{
}

void AccessInterface::request(const DataRequest& request, std::unique_ptr<ChunkPacket> payload)
{
    // Enter_Method on steroids...
    omnetpp::cMethodCallContextSwitcher ctx(mModuleOut);
    ctx.methodCall("request");

    GeoNetPacket* gn = new GeoNetPacket("GeoNet packet");
    gn->setPayload(std::move(payload));
    gn->setControlInfo(new GeoNetRequest(request));

    // gn has been created in the context of mModuleOut, thus ownership is fine
    mModuleOut->send(gn, mGateOut);
}

} // namespace artery
