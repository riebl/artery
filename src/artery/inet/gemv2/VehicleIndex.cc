/*
 * Artery V2X Simulation Framework
 * Copyright 2017 Thiago Vieira, Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/inet/gemv2/VehicleIndex.h"
#include "artery/inet/gemv2/Visualizer.h"
#include "traci/Core.h"
#include "traci/BasicNodeManager.h"
#include "traci/LiteAPI.h"
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/register/linestring.hpp>
#include <boost/geometry/strategies/transform/matrix_transformers.hpp>
#include <boost/range/adaptor/indexed.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/units/cmath.hpp>
#include <inet/common/ModuleAccess.h>
#include <omnetpp/checkandcast.h>
#include <algorithm>
#include <array>

namespace { using LineOfSight = std::array<artery::Position, 2>; }
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

    mVisualizer = inet::findModuleFromPar<Visualizer>(par("visualizerModule"), this, false);
    mVehicleMargin = std::abs(par("vehicleMargin").doubleValue());
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
        if (mVisualizer) {
            mVisualizer->drawVehicles(this);
        }
    }
}

void VehicleIndex::receiveSignal(cComponent* source, simsignal_t signal, const char* id, cObject* obj)
{
    Enter_Method_Silent();
    if (signal == traci::BasicNodeManager::addVehicleSignal) {
        traci::LiteAPI* api = check_and_cast<traci::NodeManager*>(source)->getLiteAPI();
        ASSERT(api);
        Vehicle vehicle(*api, id, mVehicleMargin);
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

bool VehicleIndex::anyBlockage(const Position& a, const Position& b, double height) const
{
    ASSERT(!mRtreeTainted);
    const LineOfSight los { a, b };
    auto rtree_intersect = bg::index::intersects(los);
    return std::any_of(mVehicleRtree.qbegin(rtree_intersect), mVehicleRtree.qend(),
            [&](const RtreeValue& candidate) {
                const Vehicle& vehicle = candidate.second->second;
                const std::vector<Position>& outline = vehicle.getOutline();
                return vehicle.getHeight() > height && bg::relate(los, outline, cutting);
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

VehicleIndex::Vehicle::Vehicle(traci::LiteAPI& api, const std::string& id, double margin) :
    mBoundary(api.simulation().getNetBoundary()), mHeight(0.0)
{
    auto vtype = api.vehicle().getTypeID(id);
    mHeight = api.vehicletype().getHeight(vtype);
    createLocalOutline(api.vehicletype().getWidth(vtype), api.vehicletype().getLength(vtype), margin);
    update(api.vehicle().getPosition(id), traci::TraCIAngle { api.vehicle().getAngle(id) });
}

void VehicleIndex::Vehicle::update(const traci::TraCIPosition& pos, traci::TraCIAngle heading)
{
    mPosition = traci::position_cast(mBoundary, pos);
    mHeading = traci::angle_cast(heading);
    calculateWorldOutline();
}

void VehicleIndex::Vehicle::createLocalOutline(double width, double length, double margin)
{
    // vehicle corner points in clockwise order, center of front bumper at origin, heading east
    mLocalOutline.assign({
        Position(margin, 0.5 * width + margin),
        Position(margin, -(0.5 * width + margin)),
        Position(-(length + margin), -(0.5 * width + margin)),
        Position(-(length + margin), 0.5 * width + margin)
    });
    mLocalMidpoint = Position { -0.5 * length, 0.0 };
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

std::vector<const VehicleIndex::Vehicle*>
VehicleIndex::vehiclesEllipse(const Position& a, const Position& b, double r) const
{
    using boost::units::fmin;
    using boost::units::fmax;

    std::vector<const Vehicle*> vehicles;
    const double d = bg::distance(a, b);
    const double k = 0.5 * (r - d);

    // Are positions a and b within (theoretical) communication range?
    if (k >= 0.0) {
        // coarse approximation of ellipse's bounding box
        geometry::Box ebb;
        bg::set<bg::min_corner, 0>(ebb, fmin(a.x, b.x).value() - k); // left
        bg::set<bg::min_corner, 1>(ebb, fmin(a.y, b.y).value() - k); // top (y axis growing downwards)
        bg::set<bg::max_corner, 0>(ebb, fmax(a.x, b.x).value() + k); // right
        bg::set<bg::max_corner, 1>(ebb, fmax(a.y, b.y).value() + k); // bottom

        auto rtree_intersect = bg::index::intersects(ebb);
        for (auto it = mVehicleRtree.qbegin(rtree_intersect); it != mVehicleRtree.qend(); ++it) {
            const Vehicle& vehicle = it->second->second;
            const Position& c = vehicle.getMidpoint();
            if (bg::distance(a, c) + bg::distance(b, c) <= r) {
                // vehicle's center is within ellipse
                vehicles.push_back(&vehicle);
            }
        }
    }

    return vehicles;
}

} // namespace gemv2
} // namespace artery
