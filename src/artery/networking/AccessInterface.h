#ifndef ARTERY_ACCESSINTERFACE_H_KQEQ2A7C
#define ARTERY_ACCESSINTERFACE_H_KQEQ2A7C

#include <omnetpp/cgate.h>
#include <vanetza/access/interface.hpp>

// forward declarations
namespace omnetpp
{
class cGate;
class cSimpleModule;
} // namespace omnetpp

namespace artery
{

class AccessInterface : public vanetza::access::Interface
{
public:
    AccessInterface(omnetpp::cGate*);
    void request(const vanetza::access::DataRequest&, std::unique_ptr<vanetza::ChunkPacket>) override;

private:
    omnetpp::cGate* mGateOut;
    omnetpp::cSimpleModule* mModuleOut;
};

} // namespace artery

#endif /* ARTERY_ACCESSINTERFACE_H_KQEQ2A7C */

