/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef EVIRONMENTMODELOBJECT_H_
#define EVIRONMENTMODELOBJECT_H_

#include "artery/application/VehicleDataProvider.h"
#include "artery/envmod//Geometry.h"
#include "artery/envmod/sensor/SensorPosition.h"
#include "artery/traci/VehicleType.h"
#include "artery/utility/Geometry.h"
#include <boost/optional/optional.hpp>
#include <cstdint>
#include <memory>
#include <vector>

namespace traci { class VehicleController; }

namespace artery
{

/**
 * EnvironmentModelObject
 */
class EnvironmentModelObject : private VehicleDataProvider
{
public:
    using Length = traci::VehicleType::Length;

    /**
     * @param ctrl associated VehicleController to this object
     * @param id station ID used by this object for application messages (e.g. CAM)
     */
    EnvironmentModelObject(const traci::VehicleController*, uint32_t id);

    /**
     * Updates the internal object data.
     */
    void update();

    /**
     * Returns the polygon describing the object's outline
     * @return polygon points
     */
    const std::vector<Position>& getOutline() const { return mOutline; }

    /**
     * Returns a sensor attachment point of the vehicle object
     * @param pos logical position of sensor
     * @return sensor attachment point
     */
    const Position& getAttachmentPoint(const SensorPosition& pos) const;

    const VehicleDataProvider& getVehicleData() const;

    std::string getExternalId() const;

    /**
     * Return the centre point coord of this vehicle object
     * @return centre point
     */
    const Position& getCentrePoint() const { return mCentrePoint; }

    Length getLength() const { return mLength; }

    Length getWidth() const { return mWidth; }

    /**
     * Return outer object radius
     *
     * Object is guaranteed to lie completely in the circle described by getCentrePoint and getRadius().
     * @return outer radius
     */
    Length getRadius() const { return mRadius; }

private:
    const traci::VehicleController* mVehicleController;
    traci::VehicleType::Length mLength;
    traci::VehicleType::Length mWidth;
    traci::VehicleType::Length mRadius;
    std::vector<Position> mOutline;
    std::vector<Position> mAttachmentPoints;
    Position mCentrePoint;
};

} // namespace artery

#endif /* EVIRONMENTMODELOBJECT_H_ */
