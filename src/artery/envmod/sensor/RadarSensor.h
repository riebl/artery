/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ENVMOD_RADARSENSOR_H_BCY7WDMB
#define ENVMOD_RADARSENSOR_H_BCY7WDMB

#include "artery/envmod/sensor/SensorConfiguration.h"
#include "artery/envmod/sensor/SensorDetection.h"
#include "artery/envmod/sensor/BaseSensor.h"
#include <omnetpp/ccanvas.h>
#include <memory>

namespace artery
{

class RadarSensor : public BaseSensor
{
public:
    RadarSensor();
    ~RadarSensor();

    void measurement() override;
    void setVisualization(const SensorVisualizationConfig&) override;
    const FieldOfView* getFieldOfView() const override;
    SensorPosition position() const override;
    omnetpp::SimTime getValidityPeriod() const override;
    const std::string& getSensorCategory() const override;

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
    void refreshDisplay() const override;

    SensorConfigRadar mRadarConfig;
    Updatable<SensorDetection> mLastDetection;

private:
    omnetpp::cFigure::Color mColor;
    omnetpp::cGroupFigure* mGroupFigure;
    omnetpp::cPolygonFigure* mSensorConeFigure;
    omnetpp::cGroupFigure* mLinesOfSightFigure;
    omnetpp::cGroupFigure* mObjectsFigure;
    omnetpp::cGroupFigure* mObstaclesFigure;
};

} // namespace artery

#endif /* ENVMOD_RADARSENSOR_H_BCY7WDMB */
