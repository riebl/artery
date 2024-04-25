/*
 * Artery V2X Simulation Framework
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ENVMOD_CPMSENSOR_H_
#define ENVMOD_CPMSENSOR_H_

/**
 * @file CpmSensor.h
 * @brief Implementation of Collective Perception Sensor
 */

#include "artery/envmod/sensor/BaseSensor.h"

#include <omnetpp/clistener.h>


namespace artery
{
class IdentityRegistry;
class VehicleDataProvider;
}  // namespace artery


namespace artery
{

/**
 * @brief Collective Perception Sensor implementation
 *
 * Stores objects of received Collective Perception Messages into the Local Environment Model.
 */
class CpmSensor : public BaseSensor, public omnetpp::cListener
{
public:
    void measurement() override;
    void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t, omnetpp::cObject*, omnetpp::cObject* = nullptr) override;
    omnetpp::SimTime getValidityPeriod() const override;
    SensorPosition position() const override { return SensorPosition::VIRTUAL; }
    const std::string& getSensorCategory() const override;
    const std::string getSensorName() const override { return mSensorName; }
    void setSensorName(const std::string& name) override { mSensorName = name; }
    SensorDetection detectObjects() const override;

protected:
    void initialize() override;
    void finish() override;

private:
    /**
     * @brief Identity Registry to use to map the received Perceived Objects to Global Environment Model objects
     */
    IdentityRegistry* mIdentityRegistry;
    /**
     * @brief Validity period of dectected objects
     */
    omnetpp::SimTime mValidityPeriod;
    /**
     * @brief Name of this sensor
     */
    std::string mSensorName;
};

}  // namespace artery

#endif /* ENVMOD_CPMSENSOR_H_ */
