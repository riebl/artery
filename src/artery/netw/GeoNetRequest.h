#ifndef GEONETREQUEST_H_WMCTXM3I
#define GEONETREQUEST_H_WMCTXM3I

#include <omnetpp/cobject.h>
#include <vanetza/access/data_request.hpp>

class GeoNetRequest : public omnetpp::cObject, public vanetza::access::DataRequest
{
	public:
		GeoNetRequest(const vanetza::access::DataRequest& request) :
			vanetza::access::DataRequest(request)
		{
		}
};

#endif /* GEONETREQUEST_H_WMCTXM3I */

