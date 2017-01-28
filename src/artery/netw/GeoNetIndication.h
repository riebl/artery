#ifndef GEONETINDICATION_H_RNMVCFZY
#define GEONETINDICATION_H_RNMVCFZY

#include <omnetpp/cobject.h>
#include <vanetza/net/mac_address.hpp>

class GeoNetIndication : public omnetpp::cObject
{
	public:
		vanetza::MacAddress source;
		vanetza::MacAddress destination;
};

#endif /* GEONETINDICATION_H_RNMVCFZY */

