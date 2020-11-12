#ifndef ARTERY_GEONETREQUEST_H_WMCTXM3I
#define ARTERY_GEONETREQUEST_H_WMCTXM3I

#include <omnetpp/cobject.h>
#include <vanetza/access/data_request.hpp>

namespace artery
{

class GeoNetRequest : public omnetpp::cObject, public vanetza::access::DataRequest
{
    public:
        GeoNetRequest(const vanetza::access::DataRequest& request) : vanetza::access::DataRequest(request)
        {
        }

        GeoNetRequest* dup() const override { return new GeoNetRequest(*this); }
};

} // namespace artery

#endif /* ARTERY_GEONETREQUEST_H_WMCTXM3I */

