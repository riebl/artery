#include <vanetza/dcc/data_request.hpp>
#include <vanetza/dcc/interface.hpp>
#include <vanetza/geonet/areas.hpp>
#include <vanetza/geonet/data_confirm.hpp>
#include <vanetza/geonet/data_indication.hpp>
#include <vanetza/geonet/mib.hpp>
#include <vanetza/geonet/packet.hpp>
#include <vanetza/geonet/router.hpp>
#include <vanetza/geonet/timestamp.hpp>
#include <vanetza/geonet/tests/network_topology.hpp>
#include <vanetza/net/mac_address.hpp>
#include <boost/optional.hpp>
#include <GeographicLib/Geocentric.hpp>
#include <GeographicLib/Geodesic.hpp>
#include <GeographicLib/LocalCartesian.hpp>
#include <list>
#include <unordered_map>

namespace vanetza
{
namespace geonet
{

std::unique_ptr<UpPacket> duplicate_copy_construct(const ChunkPacket& packet)
{
    return std::unique_ptr<UpPacket> { new UpPacket(packet) };
}

std::unique_ptr<UpPacket> duplicate_serialize(const ChunkPacket& packet)
{
    ByteBuffer buf_packet;
    for (auto layer : osi_layer_range<OsiLayer::Network, OsiLayer::Application>()) {
        ByteBuffer buf_layer;
        packet[layer].convert(buf_layer);
        buf_packet.insert(buf_packet.end(), buf_layer.begin(), buf_layer.end());
    }
    assert(buf_packet.size() == packet.size(OsiLayer::Network, OsiLayer::Application));

    return std::unique_ptr<UpPacket> { new UpPacket(CohesivePacket(std::move(buf_packet), OsiLayer::Network)) };
}

NetworkTopology::RequestInterface::RequestInterface(NetworkTopology& network, const MacAddress& mac) :
    network(network), address(mac)
{
}

void NetworkTopology::RequestInterface::request(const dcc::DataRequest& req, std::unique_ptr<ChunkPacket> packet)
{
    last_request = req;
    last_packet.reset(new ChunkPacket(*packet));
    last_request.source = address;
    network.save_request(last_request, std::move(packet));
}

NetworkTopology::RouterContext::RouterContext(NetworkTopology& network) :
    request_interface(network, mac_address),
    runtime(network.now),
    security(runtime),
    router(runtime, network.get_mib())
{
    router.set_access_interface(&request_interface);
    router.set_security_entity(&security.entity());
}

NetworkTopology::NetworkTopology() : now(Clock::at("2016-02-29 23:59"))
{
    set_duplication_mode(PacketDuplicationMode::COPY_CONSTRUCT);
    assert(fn_duplicate);
}

boost::optional<NetworkTopology::RouterContext&> NetworkTopology::get_host(const MacAddress& addr)
{
    boost::optional<RouterContext&> context;
    auto found = hosts.find(addr);
    if (found != hosts.end())
        context = *found->second;

    return context;
}

boost::optional<Router&> NetworkTopology::get_router(const MacAddress& addr)
{
    boost::optional<Router&> router;
    auto context = get_host(addr);
    if (context)
        router = context->router;

    return router;
}

boost::optional<NetworkTopology::RequestInterface&> NetworkTopology::get_interface(const MacAddress& addr)
{
    boost::optional<NetworkTopology::RequestInterface&> interface;
    auto context = get_host(addr);
    if (context)
        interface = context->request_interface;

    return interface;
}

const unsigned& NetworkTopology::get_counter_requests(const MacAddress& addr)
{
    return counter_requests[addr];
}

void NetworkTopology::add_router(const MacAddress& addr)
{
    std::unique_ptr<RouterContext> context { new RouterContext(*this) };
    context->mac_address = addr;
    context->router.set_address(Address(context->mac_address));

    hosts.emplace(addr, std::move(context));
}

void NetworkTopology::add_reachability(const MacAddress& addr, std::initializer_list<MacAddress> new_reachables)
{
    // save reachable routers in reachability map
    std::set<MacAddress>& reachables = reachability[addr];
    for (const MacAddress& new_reachable : new_reachables) {
        reachables.insert(new_reachable);
    }
}

void NetworkTopology::save_request(const dcc::DataRequest& req, std::unique_ptr<ChunkPacket> packet)
{
    // save request with packet in list requests
    requests.emplace_back(req, std::move(packet));

    // increment request counter
    counter_requests[req.source]++;
}

void NetworkTopology::dispatch()
{
    // process a stable sequence of saved requests
    decltype(requests) current_requests;
    std::swap(current_requests, requests);
    for (auto& tuple: current_requests) {
        // extract request and packet from tuple
        auto req = std::get<0>(tuple);
        get_interface(req.source)->last_packet.reset(new ChunkPacket(*std::get<1>(tuple)));

        auto neighbours = reachability[req.source];
        // broadcast packet to all reachable routers
        if (req.destination == cBroadcastMacAddress) {
            for (auto& mac_addy: neighbours) {
                send(req.source, mac_addy);
            }
        }
        // send packet only to specific destination router
        else if (neighbours.find(req.destination) != neighbours.end()) {
            send(req.source, req.destination);
        }
    }
}

void NetworkTopology::send(const MacAddress& sender, const MacAddress& destination)
{
    counter_indications++;
    std::unique_ptr<UpPacket> packet_up = fn_duplicate(*get_interface(sender)->last_packet);
    auto router = get_router(destination);
    if (router) router->indicate(std::move(packet_up), sender, destination);
}

void NetworkTopology::set_position(const MacAddress& addr, CartesianPosition c)
{
    // convert cartesian to geodetic position
    GeodeticPosition pos = convert_cartesian_geodetic(c);
    auto router = get_router(addr);
    if (router) {
        LongPositionVector lpv = router->get_local_position_vector();
        lpv.timestamp = now;
        lpv.latitude = geo_angle_i32t(pos.latitude);
        lpv.longitude = geo_angle_i32t(pos.longitude);
        router->update(lpv);
    }
}

void NetworkTopology::advance_time(Clock::duration t)
{
    const Clock::duration max_step = std::chrono::milliseconds(500);
    do {
        const auto step = std::min(t, max_step);
        now += step;
        t -= step;
        // update timestamp for every router
        for (auto& host : hosts) {
            Runtime& runtime = host.second->runtime;
            runtime.trigger(now);
            Router& router = host.second->router;
            LongPositionVector lpv = router.get_local_position_vector();
            lpv.timestamp = now;
            router.update(lpv);
        }
        dispatch();
    } while (t.count() > 0);
}

void NetworkTopology::reset_counters()
{
    counter_indications = 0;
    counter_requests.clear();
}

void NetworkTopology::set_duplication_mode(PacketDuplicationMode mode)
{
    switch (mode) {
        case PacketDuplicationMode::COPY_CONSTRUCT:
            fn_duplicate = &duplicate_copy_construct;
            break;
        case PacketDuplicationMode::SERIALIZE:
            fn_duplicate = &duplicate_serialize;
            break;
        default:
            throw std::runtime_error("Invalid PacketDuplicationMode");
            break;
    }
}

GeodeticPosition convert_cartesian_geodetic(const CartesianPosition& cart)
{
    using namespace vanetza::units;
    using namespace vanetza::units::si;

    const GeographicLib::Geocentric& earth = GeographicLib::Geocentric::WGS84();
    double lat = 0.0, lon = 0.0, unused_h = 0.0;
    GeographicLib::LocalCartesian proj(lat, lon, unused_h, earth);
    double x = cart.x / meter;
    double y = cart.y / meter;
    double unused_z = 0.0;
    proj.Reverse(x, y, unused_z, lat, lon, unused_h);

    return GeodeticPosition(lat * degree, lon * degree);
}

Area circle_dest_area(units::Length radius, units::Length midpoint_x, units::Length midpoint_y)
{
    using namespace vanetza::units;
    using namespace vanetza::units::si;

    Area dest_area;
    Circle c;
    c.r = radius;
    dest_area.shape = c;
    dest_area.position = convert_cartesian_geodetic(CartesianPosition(midpoint_x, midpoint_y));

    return dest_area;
}

} // namespace geonet
} // namespace vanetza
