/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/envmod/GlobalEnvironmentModel.h"
#include "artery/envmod/sensor/FovSensor.h"
#include "artery/envmod/sensor/SensorDetection.h"
#include "artery/envmod/LocalEnvironmentModel.h"
#include "artery/envmod/EnvironmentModelObstacle.h"
#include <boost/geometry/geometries/register/linestring.hpp>
#include <unordered_set>

using namespace omnetpp;

using LineOfSight = std::array<artery::Position, 2>;
BOOST_GEOMETRY_REGISTER_LINESTRING(LineOfSight)

namespace artery
{

FovSensor::FovSensor() :
    mGroupFigure(nullptr), mSensorConeFigure(nullptr), mLinesOfSightFigure(nullptr),
    mObjectsFigure(nullptr), mObstaclesFigure(nullptr)
{
}

FovSensor::~FovSensor()
{
    if (mGroupFigure) {
        delete mGroupFigure->removeFromParent();
    }
}

void FovSensor::initialize()
{
    BaseSensor::initialize();

    std::string groupName = getEgoId() + "-" + getSensorName();
    mGroupFigure = new cGroupFigure(groupName.c_str());
    mGlobalEnvironmentModel->getCanvas()->addFigure(mGroupFigure);
    mColor = cFigure::GOOD_DARK_COLORS[getId() % cFigure::NUM_GOOD_DARK_COLORS];

    mFovConfig.egoID = getEgoId();
    mFovConfig.sensorID = getId();
    mFovConfig.sensorPosition = determineSensorPosition(par("attachmentPoint"));

    mFovConfig.fieldOfView.range = par("fovRange").doubleValue() * boost::units::si::meters;
    mFovConfig.fieldOfView.angle = par("fovAngle").doubleValue() * boost::units::degree::degrees;
    mFovConfig.numSegments = par("numSegments");
    mFovConfig.doLineOfSightCheck = par("doLineOfSightCheck");

    initializeVisualization();
}

void FovSensor::measurement()
{
    Enter_Method("measurement");
    auto objects = mGlobalEnvironmentModel->detectObjects(std::bind(&Sensor::detectObjects, this, std::placeholders::_1, std::placeholders::_2));
    mLocalEnvironmentModel->complementObjects(objects, *this);
    mLastDetection = std::move(objects);
}

SensorDetection FovSensor::detectObjects(ObstacleRtree& obstacleRtree, PreselectionMethod& preselector) const {

    namespace bg = boost::geometry;
    if (mFovConfig.fieldOfView.range <= 0.0 * boost::units::si::meter) {
        throw std::runtime_error("sensor range is 0 meter or less");
    } else if (mFovConfig.fieldOfView.angle > 360.0 * boost::units::degree::degrees) {
        throw std::runtime_error("sensor opening angle exceeds 360 degree");
    }

    SensorDetection detection;

    // create sensor cone
    const auto& egoObj = mGlobalEnvironmentModel->getObject(mFovConfig.egoID);
    if (!egoObj) {
        throw std::runtime_error("no object found for ID " + mFovConfig.egoID);
    }
    detection.sensorCone = createSensorArc(mFovConfig, *egoObj);

    std::vector<std::string> preselObjectsInSensorRange = preselector.select(*egoObj, mFovConfig);

    // get obstacles intersecting with sensor cone
    std::vector<ObstacleRtreeValue> obstacleIntersections;
    geometry::Polygon tmp; /*< Boost 1.61 fails when detection.sensorCone is used directly in R-Tree query */
    bg::convert(detection.sensorCone, tmp);
    obstacleRtree.query(bg::index::intersects(tmp), std::back_inserter(obstacleIntersections));

    const auto& egoPointPosition =  mGlobalEnvironmentModel->getObject(mFovConfig.egoID)->getAttachmentPoint(mFovConfig.sensorPosition);

    if (mFovConfig.doLineOfSightCheck)
    {
        std::unordered_set<std::shared_ptr<EnvironmentModelObstacle>> blockingObstacles;

        // check if objects in sensor cone are hidden by another object or an obstacle
        for (const auto& objectId : preselObjectsInSensorRange)
        {
            const auto& object =  mGlobalEnvironmentModel->getObject(objectId);
            for (const auto& objectPoint : object->getOutline())
            {
                // skip objects points outside of sensor cone
                if (!bg::covered_by(objectPoint, detection.sensorCone)) {
                    continue;
                }

                LineOfSight lineOfSight;
                lineOfSight[0] = egoPointPosition;
                lineOfSight[1] = objectPoint;

                bool noVehicleOccultation = std::none_of(preselObjectsInSensorRange.begin(), preselObjectsInSensorRange.end(),
                        [&](const std::string& objectId) {
                            const std::vector<Position>& objectOutline =  mGlobalEnvironmentModel->getObject(objectId)->getOutline();
                            return bg::crosses(lineOfSight, objectOutline);
                        });

                bool noObstacleOccultation = std::none_of(obstacleIntersections.begin(), obstacleIntersections.end(),
                        [&](const ObstacleRtreeValue& obstacleIntersection) {
                            const auto& obstacle = mGlobalEnvironmentModel->getObstacle(obstacleIntersection.second);
                            ASSERT(obstacle);
                            if (bg::intersects(lineOfSight, obstacle->getOutline())) {
                                blockingObstacles.insert(obstacle);
                                return true;
                            } else {
                                return false;
                            }
                        });

                if (noVehicleOccultation && noObstacleOccultation) {
                    if (detection.objects.empty() || detection.objects.back() != object) {
                        detection.objects.push_back(object);
                    }

                    if (mDrawLinesOfSight) {
                        detection.visiblePoints.push_back(objectPoint);
                    } else {
                        // no need to check other object points in detail except for visualization
                        break;
                    }
                }
            } // for each (corner) point of object polygon
        } // for each object

        detection.obstacles.assign(blockingObstacles.begin(), blockingObstacles.end());
    } else {
        for (const auto& objectId : preselObjectsInSensorRange) {
            detection.objects.push_back(mGlobalEnvironmentModel->getObject(objectId));
        }
    }

    return detection;

}

void FovSensor::initializeVisualization()
{
    assert(mGroupFigure);
    mDrawLinesOfSight = par("drawLinesOfSight");
    bool drawSensorCone = par("drawSensorCone");
    bool drawObjects = par("drawDetectedObjects");
    bool drawObstacles = par("drawBlockingObstacles");

    if (drawSensorCone && !mSensorConeFigure) {
        mSensorConeFigure = new cPolygonFigure("sensor cone");
        mSensorConeFigure->setLineColor(mColor);
        mGroupFigure->addFigure(mSensorConeFigure);
    } else if (!drawSensorCone && mSensorConeFigure) {
        delete mSensorConeFigure->removeFromParent();
        mSensorConeFigure = nullptr;
    }

    if(mDrawLinesOfSight && !mLinesOfSightFigure) {
        mLinesOfSightFigure = new cGroupFigure("lines of sight");
        mGroupFigure->addFigure(mLinesOfSightFigure);
    } else if (!mDrawLinesOfSight && mLinesOfSightFigure) {
        delete mLinesOfSightFigure->removeFromParent();
        mLinesOfSightFigure = nullptr;
    }

    if (drawObstacles && !mObstaclesFigure) {
        mObstaclesFigure = new cGroupFigure("obstacles");
        mGroupFigure->addFigure(mObstaclesFigure);
    } else if (!drawObstacles && mObstaclesFigure) {
        delete mObstaclesFigure->removeFromParent();
        mObstaclesFigure = nullptr;
    }

    if (drawObjects && !mObjectsFigure) {
        mObjectsFigure = new cGroupFigure("objects");
        mGroupFigure->addFigure(mObjectsFigure);
    } else if (!drawObjects && mObjectsFigure) {
        delete mObjectsFigure->removeFromParent();
        mObjectsFigure = nullptr;
    }
}

const FieldOfView& FovSensor::getFieldOfView() const
{
    return mFovConfig.fieldOfView;
}

omnetpp::SimTime FovSensor::getValidityPeriod() const
{
    using namespace omnetpp;
    return SimTime { 200, SIMTIME_MS };
}

SensorPosition FovSensor::position() const
{
    return mFovConfig.sensorPosition;
}

const std::string& FovSensor::getSensorCategory() const
{
    static const std::string category = "FoV";
    return category;
}

const std::string FovSensor::getSensorName() const
{
    return mFovConfig.sensorName;
}

void FovSensor::setSensorName(const std::string& name)
{
    mFovConfig.sensorName = name;
}

void FovSensor::refreshDisplay() const
{
    if (!mLastDetection) {
        return;
    }

    if (mSensorConeFigure) {
        std::vector<cFigure::Point> points;
        for (const auto& position : mLastDetection->sensorCone) {
            points.push_back(cFigure::Point { position.x.value(), position.y.value() });
        }
        mSensorConeFigure->setPoints(points);
    }

    if (mLinesOfSightFigure) {
        // remove previous lines
        while (mLinesOfSightFigure->getNumFigures() > 0) {
            delete mLinesOfSightFigure->removeFigure(0);
        }

        const Position& startPoint = mLastDetection->sensorCone.front();

        for (const Position& endPoint : mLastDetection->visiblePoints) {
            auto line = new cLineFigure();
            line->setLineColor(mColor);
            line->setLineStyle(cFigure::LINE_DASHED);
            line->setStart(cFigure::Point { startPoint.x.value(), startPoint.y.value() });
            line->setEnd(cFigure::Point { endPoint.x.value(), endPoint.y.value() });
            mLinesOfSightFigure->addFigure(line);
        }
    }

    if (mObstaclesFigure) {
        // remove previous obstacles
        while (mObstaclesFigure->getNumFigures() > 0) {
            delete mObstaclesFigure->removeFigure(0);
        }

        for (const auto& obstacle : mLastDetection->obstacles) {
            auto polygon = new cPolygonFigure(obstacle->getObstacleId().c_str());
            polygon->setFilled(true);
            polygon->setFillColor(mColor);
            polygon->setLineColor(cFigure::BLUE);
            for (const auto& position : obstacle->getOutline()) {
                polygon->addPoint(cFigure::Point { position.x.value(), position.y.value() });
            }
            mObstaclesFigure->addFigure(polygon);
        }
    }

    if (mObjectsFigure) {
        // remove previous objects
        while (mObjectsFigure->getNumFigures() > 0) {
            delete mObjectsFigure->removeFigure(0);
        }

        for (const auto& object : mLastDetection->objects) {
            auto polygon = new cPolygonFigure(object->getExternalId().c_str());
            polygon->setFilled(true);
            polygon->setFillColor(mColor);
            polygon->setLineColor(cFigure::RED);
            for (const auto& position : object->getOutline()) {
                polygon->addPoint(cFigure::Point { position.x.value(), position.y.value() });
            }
            mObjectsFigure->addFigure(polygon);
        }
    }
}

} // namespace artery
