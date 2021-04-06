/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ENVMOD_FOVRSENSOR_H_BCY7WDMB
#define ENVMOD_FOVRSENSOR_H_BCY7WDMB

#include "artery/envmod/sensor/SensorConfiguration.h"
#include "artery/envmod/sensor/SensorDetection.h"
#include "artery/envmod/sensor/BaseSensor.h"
#include <omnetpp/ccanvas.h>
#include <memory>
#include <functional>

namespace artery
{

class FovSensor : public BaseSensor
{
public:
    FovSensor();
    ~FovSensor();

    void measurement() override;
    const FieldOfView& getFieldOfView() const;
    SensorPosition position() const override;
    omnetpp::SimTime getValidityPeriod() const override;
    const std::string& getSensorCategory() const override;
    const std::string getSensorName() const override;
    void setSensorName(const std::string& name) override;
    SensorDetection detectObjects(ObstacleRtree& obstacleRtree, PreselectionMethod& preselector) const override;

protected:
    template<typename T>
    class Updatable
    {
    public:
        void operator=(T&& t) { mValue = std::move(t); mFlag = true; }
        operator bool() const { bool tmp = mFlag; mFlag = false; return tmp; }
        const T* operator->() const { return &mValue; }
        const T& operator*() const { return mValue; }

    private:
        mutable bool mFlag = false;
        T mValue;
    };

    void initialize() override;
    void initializeVisualization();
    void refreshDisplay() const override;

    SensorConfigFov mFovConfig;
    Updatable<SensorDetection> mLastDetection;
    bool mDrawLinesOfSight;

private:
    omnetpp::cFigure::Color mColor;
    omnetpp::cGroupFigure* mGroupFigure;
    omnetpp::cPolygonFigure* mSensorConeFigure;
    omnetpp::cGroupFigure* mLinesOfSightFigure;
    omnetpp::cGroupFigure* mObjectsFigure;
    omnetpp::cGroupFigure* mObstaclesFigure;
};

} // namespace artery

#endif /* ENVMOD_FOVRSENSOR_H_BCY7WDMB */
