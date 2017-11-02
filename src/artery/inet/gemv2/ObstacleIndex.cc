/*
 * Artery V2X Simulation Framework
 * Copyright 2017 Thiago Vieira, Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/inet/gemv2/ObstacleIndex.h"
#include "traci/Core.h"
#include "traci/LiteAPI.h"
#include <boost/algorithm/string.hpp>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/register/linestring.hpp>
#include <boost/range/adaptor/indexed.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <omnetpp/checkandcast.h>
#include <algorithm>
#include <array>

namespace { using LineOfSight = std::array<Position, 2>; }
BOOST_GEOMETRY_REGISTER_LINESTRING(LineOfSight)

namespace artery
{
namespace gemv2
{

Define_Module(ObstacleIndex)

using namespace omnetpp;
namespace bg = boost::geometry;

namespace {
    const simsignal_t traciInitSignal = cComponent::registerSignal("traci.init");
}


void ObstacleIndex::initialize()
{
    cModule* traci = getModuleByPath(par("traciModule"));
    if (traci) {
        traci->subscribe(traciInitSignal, this);
    } else {
        throw cRuntimeError("No TraCI module found for signal subscription");
    }

    mFilterTypes.clear();
    const std::string filterTypes = par("filterTypes");
    boost::split(mFilterTypes, filterTypes, boost::is_any_of(" "));
}

void ObstacleIndex::receiveSignal(cComponent* source, simsignal_t signal, const SimTime&, cObject*)
{
    Enter_Method_Silent();
    if (signal == traciInitSignal) {
        auto core = check_and_cast<traci::Core*>(source);
        fetchObstacles(core->getLiteAPI());
    }
}

void ObstacleIndex::fetchObstacles(traci::LiteAPI& traci)
{
    const auto& polygons = traci.polygon();
    const auto boundary = traci.simulation().getNetBoundary();
    unsigned ignored = 0;
    std::string shape_msg;
    for (const std::string& id : polygons.getIDList()) {
        if (!mFilterTypes.empty()) {
            std::string type = polygons.getType(id);
            auto found = mFilterTypes.find(type);
            if (found == mFilterTypes.end()) {
                EV_DEBUG << "ignore polygon " << id << " of type " << type << "\n";
                // skip polygon because its type is not in our filter set
                ++ignored;
                continue;
            }
        }

        std::vector<Position> shape;
        for (const traci::TraCIPosition& point : polygons.getShape(id)) {
            bg::append(shape, traci::position_cast(boundary, point));
        }

        bg::correct(shape); // fixes issues such as reversed point order
        if (!bg::is_valid(shape, shape_msg)) {
            EV_DEBUG << "ignore invalid polygon " << id << " (" << shape_msg << ")\n";
            ++ignored;
            continue;
        }

        mObstacles.emplace_back(std::move(shape));
    }

    struct rtree_value_maker
    {
        RtreeValue operator()(const boost::range::index_value<Obstacle&>& v) const
        {
            using Indexable = typename RtreeValue::first_type;
            return RtreeValue { bg::return_envelope<Indexable>(v.value().getOutline()), v.index() };
        }
    };

    using namespace boost::adaptors;
    Rtree tree { mObstacles | indexed() | transformed(rtree_value_maker()) };
    mObstacleRtree = std::move(tree);
    EV_INFO << mObstacles.size() << " obstacles stored (" << ignored << " ignored)\n";
}

bool ObstacleIndex::anyBlockage(const Position& a, const Position& b) const
{
    const LineOfSight los { a, b };
    auto rtree_intersect = bg::index::intersects(los);
    return std::any_of(mObstacleRtree.qbegin(rtree_intersect), mObstacleRtree.qend(),
            [&](const RtreeValue& candidate) {
                return bg::intersects(los, mObstacles[candidate.second].getOutline());
            });
}

ObstacleIndex::Obstacle::Obstacle(std::vector<Position>&& shape) :
    mOutline(shape),
    mCentroid(bg::return_centroid<Position>(mOutline)),
    mArea(bg::area(mOutline))
{
}

} // namespace gemv2
} // namespace artery
