#ifndef REPETITION_DISPATCHER_HPP_GO4BAEVL
#define REPETITION_DISPATCHER_HPP_GO4BAEVL

#include <vanetza/geonet/router.hpp>
#include <boost/variant/static_visitor.hpp>

namespace vanetza
{
namespace geonet
{

class RepetitionDispatcher : public boost::static_visitor<void>
{
public:
    RepetitionDispatcher(Router& _router, std::unique_ptr<DownPacket> _payload)
        : router(_router), payload(std::move(_payload))
    {}

    template<class REQUEST>
    void operator()(const REQUEST& request)
    {
        router.request(request, std::move(payload));
    }

private:
    Router& router;
    std::unique_ptr<DownPacket> payload;
};

} // namespace geonet
} // namespace vanetza

#endif /* REPETITION_DISPATCHER_HPP_GO4BAEVL */

