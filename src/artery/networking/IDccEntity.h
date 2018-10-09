#ifndef ARTERY_IDCCENTITY_H_EOWR2LWL
#define ARTERY_IDCCENTITY_H_EOWR2LWL

#include <vanetza/dcc/channel_probe_processor.hpp>
#include <vanetza/dcc/interface.hpp>
#include <vanetza/dcc/transmit_rate_control.hpp>
#include <vanetza/geonet/dcc_field_generator.hpp>

namespace artery
{

class IDccEntity
{
public:
    virtual void reportLocalChannelLoad(vanetza::dcc::ChannelLoad) = 0;

    virtual vanetza::dcc::ChannelProbeProcessor* getChannelProbeProcessor() = 0;
    virtual vanetza::dcc::RequestInterface* getRequestInterface() = 0;
    virtual vanetza::dcc::TransmitRateThrottle* getTransmitRateThrottle() = 0;
    virtual vanetza::geonet::DccFieldGenerator* getGeonetFieldGenerator() = 0;

    virtual ~IDccEntity() = default;
};

} // namespace artery

#endif /* ARTERY_IDCCENTITY_H_EOWR2LWL */

