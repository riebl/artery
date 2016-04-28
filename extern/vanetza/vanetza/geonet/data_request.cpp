#include "data_request.hpp"
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

} // namespace geonet
} // namespace vanetza
