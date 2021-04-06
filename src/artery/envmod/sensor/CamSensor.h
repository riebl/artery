/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ENVMOD_CAMSENSOR_H_
#define ENVMOD_CAMSENSOR_H_

#include "artery/envmod/sensor/BaseSensor.h"
#include <vanetza/asn1/cam.hpp>
#include <omnetpp/clistener.h>

namespace artery
{

class IdentityRegistry;

class CamSensor : public BaseSensor, public omnetpp::cListener
{
public:
    void measurement() override;
    void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t, omnetpp::cObject*, omnetpp::cObject* = nullptr) override;
    omnetpp::SimTime getValidityPeriod() const override;
    SensorPosition position() const override { return SensorPosition::VIRTUAL; }
    const std::string& getSensorCategory() const override;
    const std::string getSensorName() const override {  return mSensorName; }
    void setSensorName(const std::string& name) override { mSensorName = name; }
    SensorDetection detectObjects(ObstacleRtree&, PreselectionMethod&) const override;

protected:
    void initialize() override;
    void finish() override;

private:
    IdentityRegistry* mIdentityRegistry;
    omnetpp::SimTime mValidityPeriod;
    std::string mSensorName;
};

} // namespace artery

#endif /* ENVMOD_CAMSENSOR_H_ */
