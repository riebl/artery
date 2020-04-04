/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/envmod/sensor/RadarSensor.h"
#include "artery/envmod/sensor/SensorDetection.h"
#include "artery/envmod/GlobalEnvironmentModel.h"
#include "artery/envmod/LocalEnvironmentModel.h"
#include "artery/envmod/EnvironmentModelObstacle.h"

using namespace omnetpp;

namespace artery
{

RadarSensor::RadarSensor() :
    mGroupFigure(nullptr), mSensorConeFigure(nullptr), mLinesOfSightFigure(nullptr),
    mObjectsFigure(nullptr), mObstaclesFigure(nullptr)
{
}

RadarSensor::~RadarSensor()
{
    if (mGroupFigure) {
        delete mGroupFigure->removeFromParent();
    }
}

void RadarSensor::initialize()
{
    BaseSensor::initialize();

    mGroupFigure = new cGroupFigure(getEgoId().c_str());
    mGlobalEnvironmentModel->getCanvas()->addFigure(mGroupFigure);
    mColor = cFigure::GOOD_DARK_COLORS[getId() % cFigure::NUM_GOOD_DARK_COLORS];

    mRadarConfig.fieldOfView.range = par("fovRange").doubleValue() * boost::units::si::meters;
    mRadarConfig.fieldOfView.angle = par("fovAngle").doubleValue() * boost::units::degree::degrees;
    mRadarConfig.numSegments = par("numSegments");
}

void RadarSensor::measurement()
{
    Enter_Method("measurement");
    auto objects = mGlobalEnvironmentModel->detectObjects(mRadarConfig);
    mLocalEnvironmentModel->complementObjects(objects, *this);
    mLastDetection = std::move(objects);
}

void RadarSensor::setVisualization(const SensorVisualizationConfig& config)
{
    assert(mGroupFigure);
    mRadarConfig.visualizationConfig = config;

    if(config.sensorCone && !mSensorConeFigure) {
        mSensorConeFigure = new cPolygonFigure("sensor cone");
        mSensorConeFigure->setLineColor(mColor);
        mGroupFigure->addFigure(mSensorConeFigure);
    } else if (!config.sensorCone && mSensorConeFigure) {
        delete mSensorConeFigure->removeFromParent();
        mSensorConeFigure = nullptr;
    }

    if(config.linesOfSight && !mLinesOfSightFigure) {
        mLinesOfSightFigure = new cGroupFigure("lines of sight");
        mGroupFigure->addFigure(mLinesOfSightFigure);
    } else if (!config.linesOfSight && mLinesOfSightFigure) {
        delete mLinesOfSightFigure->removeFromParent();
        mLinesOfSightFigure = nullptr;
    }

    if (config.obstaclesInSensorRange && !mObstaclesFigure) {
        mObstaclesFigure = new cGroupFigure("obstacles");
        mGroupFigure->addFigure(mObstaclesFigure);
    } else if (!config.obstaclesInSensorRange && mObstaclesFigure) {
        delete mObstaclesFigure->removeFromParent();
        mObstaclesFigure = nullptr;
    }

    if (config.objectsInSensorRange && !mObjectsFigure) {
        mObjectsFigure = new cGroupFigure("objects");
        mGroupFigure->addFigure(mObjectsFigure);
    } else if (!config.objectsInSensorRange && mObjectsFigure) {
        delete mObjectsFigure->removeFromParent();
        mObjectsFigure = nullptr;
    }
}

const FieldOfView* RadarSensor::getFieldOfView() const
{
    return &mRadarConfig.fieldOfView;
}

omnetpp::SimTime RadarSensor::getValidityPeriod() const
{
    using namespace omnetpp;
    return SimTime { 200, SIMTIME_MS };
}

SensorPosition RadarSensor::position() const
{
    return mRadarConfig.sensorPosition;
}

const std::string& RadarSensor::getSensorCategory() const
{
    static const std::string category = "Radar";
    return category;
}

void RadarSensor::refreshDisplay() const
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
