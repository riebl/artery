#ifndef POSITION_PROVIDER_HPP_AY7IPZKJ
#define POSITION_PROVIDER_HPP_AY7IPZKJ

#include <vanetza/geonet/position_vector.hpp>

class PositionProvider
{
public:
    virtual vanetza::geonet::LongPositionVector current_position() = 0;
};

#endif /* POSITION_PROVIDER_HPP_AY7IPZKJ */

