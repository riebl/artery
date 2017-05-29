#include "data_request.hpp"
#include <vanetza/btp/data_request.hpp>
#include <vanetza/units/time.hpp>
#include <boost/units/cmath.hpp>

namespace vanetza
{
namespace geonet
{

void decrement_by_one(DataRequest::Repetition& repetition)
{
    const auto zero = 0.0 * units::si::seconds;
    if (repetition.maximum > zero && repetition.interval > zero
        && repetition.maximum > repetition.interval) {
        repetition.maximum = repetition.maximum - repetition.interval;

    } else {
        repetition.maximum = zero;
    }
}

bool has_further_repetition(const DataRequest& request)
{
    bool repeat = false;
    if (request.repetition) {
        repeat = has_further_repetition(request.repetition.get());
    }
    return repeat;
}

bool has_further_repetition(const DataRequest::Repetition& repetition)
{
    const auto zero = 0.0 * units::si::seconds;
    return repetition.maximum > zero && repetition.interval > zero &&
        repetition.maximum >= repetition.interval;
}

struct access_request_visitor : public boost::static_visitor<DataRequest&>
{
    template<typename REQUEST>
    DataRequest& operator()(REQUEST& request)
    {
        return request;
    }
};

DataRequest& access_request(DataRequestVariant& variant)
{
    access_request_visitor visitor;
    return boost::apply_visitor(visitor, variant);
}

void copy_request_parameters(const btp::DataRequestB& btp, DataRequest& gn)
{
    gn.upper_protocol = geonet::UpperProtocol::BTP_B;
    gn.communication_profile = btp.gn.communication_profile;
    gn.security_profile = btp.gn.security_profile;
    if (btp.gn.maximum_lifetime) {
        gn.maximum_lifetime = *btp.gn.maximum_lifetime;
    }
    gn.repetition = btp.gn.repetition;
    if (btp.gn.maximum_hop_limit) {
        gn.max_hop_limit = *btp.gn.maximum_hop_limit;
    }
    gn.traffic_class = btp.gn.traffic_class;
}

} // namespace geonet
} // namespace vanetza
