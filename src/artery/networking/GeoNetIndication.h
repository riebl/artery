#ifndef ARTERY_GEONETINDICATION_H_RNMVCFZY
#define ARTERY_GEONETINDICATION_H_RNMVCFZY

#include <omnetpp/cobject.h>
#include <vanetza/net/mac_address.hpp>

namespace artery
{

class GeoNetIndication : public omnetpp::cObject
{
    public:
        vanetza::MacAddress source;
        vanetza::MacAddress destination;

        GeoNetIndication* dup() const override { return new GeoNetIndication(*this); }
};

} // namespace artery

#endif /* ARTERY_GEONETINDICATION_H_RNMVCFZY */

