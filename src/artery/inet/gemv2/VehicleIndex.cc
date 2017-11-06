/*
 * Artery V2X Simulation Framework
 * Copyright 2017 Thiago Vieira, Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/inet/gemv2/VehicleIndex.h"
#include "traci/Core.h"
#include "traci/BasicNodeManager.h"
#include "traci/LiteAPI.h"
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/register/linestring.hpp>
#include <boost/geometry/strategies/transform/matrix_transformers.hpp>
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

Define_Module(VehicleIndex)

using namespace omnetpp;
namespace bg = boost::geometry;

namespace {
    const simsignal_t traciInitSignal = cComponent::registerSignal("traci.init");
    const bg::de9im::mask cutting("T**FF****");
}


void VehicleIndex::initialize()
{
    cModule* traci = getModuleByPath(par("traciModule"));
    if (traci) {
        traci->subscribe(traci::BasicNodeManager::updateNodeSignal, this);
        traci->subscribe(traci::BasicNodeManager::addVehicleSignal, this);
        traci->subscribe(traci::BasicNodeManager::updateVehicleSignal, this);
        traci->subscribe(traci::BasicNodeManager::removeVehicleSignal, this);
    } else {
        throw cRuntimeError("No TraCI module found for signal subscription");
    }
}

void VehicleIndex::receiveSignal(cComponent* source, simsignal_t signal, unsigned long, cObject* obj)
{
    Enter_Method_Silent();
    if (signal == traci::BasicNodeManager::updateNodeSignal) {
        using Indexable = typename RtreeValue::first_type;
        using Iterator = typename RtreeValue::second_type;
        mVehicleRtree.clear();
        for (Iterator it = mVehicles.begin(); it != mVehicles.end(); ++it) {
            const Vehicle& vehicle = it->second;
            mVehicleRtree.insert(RtreeValue { bg::return_envelope<Indexable>(vehicle.getOutline()), it });
        }
        mRtreeTainted = false;
    }
}

void VehicleIndex::receiveSignal(cComponent* source, simsignal_t signal, const char* id, cObject* obj)
{
    Enter_Method_Silent();
    if (signal == traci::BasicNodeManager::addVehicleSignal) {
        traci::LiteAPI* api = check_and_cast<traci::NodeManager*>(source)->getLiteAPI();
        ASSERT(api);
        Vehicle vehicle(*api, id);
        mVehicles.emplace(id, std::move(vehicle));
    } else if (signal == traci::BasicNodeManager::updateVehicleSignal) {
        auto vehicle = check_and_cast<traci::BasicNodeManager::VehicleObject*>(obj);
        mVehicles.at(id).update(vehicle->getPosition(), vehicle->getHeading());
    } else if (signal == traci::BasicNodeManager::removeVehicleSignal) {
        mVehicles.erase(id);
    } else {
        return;
    }

    bool mRtreeTainted = true;
}

bool VehicleIndex::anyBlockage(const Position& a, const Position& b) const
{
    ASSERT(!mRtreeTainted);
    const LineOfSight los { a, b };
    auto rtree_intersect = bg::index::intersects(los);
    return std::any_of(mVehicleRtree.qbegin(rtree_intersect), mVehicleRtree.qend(),
            [&](const RtreeValue& candidate) {
                const Vehicle& vehicle = candidate.second->second;
                const std::vector<Position>& outline = vehicle.getOutline();
                return bg::relate(los, outline, cutting);
            });
}

std::vector<const VehicleIndex::Vehicle*>
VehicleIndex::getObstructingVehicles(const Position& a, const Position& b) const
{
    std::vector<const Vehicle*> result;
    const LineOfSight los { a, b };
    auto rtree_intersect = bg::index::intersects(los);
    for (auto it = mVehicleRtree.qbegin(rtree_intersect); it != mVehicleRtree.qend(); ++it) {
        const Vehicle& vehicle = it->second->second;
        if (bg::relate(los, vehicle.getOutline(), cutting)) {
            result.push_back(&vehicle);
        }
    }
    return result;
}

VehicleIndex::Vehicle::Vehicle(traci::LiteAPI& api, const std::string& id) :
    mBoundary(api.simulation().getNetBoundary()), mHeight(0.0)
{
    auto vtype = api.vehicle().getTypeID(id);
    mHeight = api.vehicletype().getHeight(vtype);
    createLocalOutline(api.vehicletype().getWidth(vtype), api.vehicletype().getLength(vtype));
    update(api.vehicle().getPosition(id), traci::TraCIAngle { api.vehicle().getAngle(id) });
}

void VehicleIndex::Vehicle::update(const traci::TraCIPosition& pos, traci::TraCIAngle heading)
{
    mPosition = traci::position_cast(mBoundary, pos);
    mHeading = traci::angle_cast(heading);
    calculateWorldOutline();
}

void VehicleIndex::Vehicle::createLocalOutline(double width, double length)
{
    // vehicle corner points in clockwise order, center of front bumper at origin, heading east
    mLocalOutline.assign({
        Position(0.0, 0.5 * width),
        Position(0.0, -0.5 * width),
        Position(-length, -0.5 * width),
        Position(-length, 0.5 * width)
    });
    mLocalMidpoint = Position { -0.5*length, 0.0 };
}

void VehicleIndex::Vehicle::calculateWorldOutline()
{
    using namespace boost::geometry::strategy::transform;
    std::vector<Position> tmp;
    mWorldOutline.clear();

    rotate_transformer<boost::geometry::radian, double, 2, 2> rot(mHeading.radian());
    translate_transformer<double, 2, 2> mov(mPosition.x.value(), mPosition.y.value());
    boost::geometry::transform(mLocalOutline, tmp, rot);
    boost::geometry::transform(tmp, mWorldOutline, mov);
    tmp.resize(1);
    boost::geometry::transform(mLocalMidpoint, tmp.front(), rot);
    boost::geometry::transform(tmp.front(), mWorldMidpoint, mov);

    ASSERT(mWorldOutline.size() == mLocalOutline.size());
    ASSERT(bg::is_valid(mWorldOutline));
}

} // namespace gemv2
} // namespace artery
