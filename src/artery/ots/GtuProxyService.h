#ifndef ARTERY_GTUPROXYSERVICE_H_HA37ALAM
#define ARTERY_GTUPROXYSERVICE_H_HA37ALAM

#include "artery/application/ItsG5Service.h"
#include "ots/RadioEndpoint.h"
#include <string>

// forward declarations
namespace ots { class Core; }

namespace artery
{

class GtuProxyService : public ItsG5Service, public ots::RadioEndpoint
{
public:
    ~GtuProxyService();
    void initialize() override;
    void indicate(const vanetza::btp::DataIndication&, omnetpp::cPacket*) override;
    void onRadioTransmit(std::unique_ptr<ots::RadioMessage>) override;

private:
    ots::Core* mCore = nullptr;
    int mItsAid = 0;
    std::string mGtuId;
};

} // namespace artery

#endif /* ARTERY_GTUPROXYSERVICE_H_HA37ALAM */