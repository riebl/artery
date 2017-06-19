#include <vanetza/common/clock.hpp>
#include <vanetza/common/runtime.hpp>
#include <vanetza/dcc/data_request.hpp>
#include <vanetza/dcc/interface.hpp>
#include <vanetza/geonet/areas.hpp>
#include <vanetza/geonet/mib.hpp>
#include <vanetza/geonet/packet.hpp>
#include <vanetza/geonet/router.hpp>
#include <vanetza/geonet/tests/security_context.hpp>
#include <vanetza/net/mac_address.hpp>
#include <vanetza/units/length.hpp>
#include <boost/optional.hpp>
#include <functional>
#include <initializer_list>
#include <list>
#include <set>
#include <unordered_map>

namespace vanetza
{
namespace geonet
{

class NetworkTopology
{
public:
    enum class PacketDuplicationMode
    {
        COPY_CONSTRUCT,
        SERIALIZE
    };

    class RequestInterface : public dcc::RequestInterface
    {
    public:
        RequestInterface(NetworkTopology&, const MacAddress&);
        void request(const dcc::DataRequest&, std::unique_ptr<ChunkPacket>) override;

        dcc::DataRequest last_request;
        std::unique_ptr<ChunkPacket> last_packet;

    private:
        NetworkTopology& network;
        const MacAddress& address;
    };

    class RouterContext
    {
    public:
        RouterContext(NetworkTopology&);

        MacAddress mac_address;
        RequestInterface request_interface;
        Runtime runtime;
        SecurityContext security;
        Router router;
    };

    NetworkTopology();
    boost::optional<RouterContext&> get_host(const MacAddress&);
    boost::optional<Router&> get_router(const MacAddress&);
    boost::optional<RequestInterface&> get_interface(const MacAddress&);
    const unsigned& get_counter_requests(const MacAddress&);
    const unsigned& get_counter_indications() const { return counter_indications; }
    ManagementInformationBase& get_mib() { return mib; }
    void add_router(const MacAddress&);
    void add_reachability(const MacAddress&, std::initializer_list<MacAddress>);
    void save_request(const dcc::DataRequest&, std::unique_ptr<ChunkPacket>);
    void dispatch();
    void send(const MacAddress&, const MacAddress&);
    void set_position(const MacAddress&, CartesianPosition);
    void advance_time(Clock::duration);
    void reset_counters();
    void set_duplication_mode(PacketDuplicationMode);

private:
    std::unordered_map<MacAddress, unsigned> counter_requests;
    std::unordered_map<MacAddress, std::unique_ptr<RouterContext>> hosts;
    std::unordered_map<MacAddress, std::set<MacAddress>> reachability;
    std::list<std::tuple<dcc::DataRequest, std::unique_ptr<ChunkPacket>>> requests;
    Clock::time_point now;
    ManagementInformationBase mib;
    unsigned counter_indications;
    std::function<std::unique_ptr<UpPacket>(const ChunkPacket&)> fn_duplicate;
};

GeodeticPosition convert_cartesian_geodetic(const CartesianPosition&);
Area circle_dest_area(units::Length radius, units::Length midpoint_x, units::Length midpoint_y);

} // namespace geonet
} // namespace vanetza
