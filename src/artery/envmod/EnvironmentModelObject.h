/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef EVIRONMENTMODELOBJECT_H_
#define EVIRONMENTMODELOBJECT_H_

#include "artery/envmod/sensor/SensorPosition.h"
#include "artery/utility/Geometry.h"
#include <vanetza/units/length.hpp>
#include <cstdint>
#include <memory>
#include <vector>

namespace artery
{

/**
 * EnvironmentModelObject is the interface class for all dynamic objects
 * tracked by the GlobalEnvironmentModel and thus detectable by sensors.
 */
class EnvironmentModelObject
{
public:
    using Length = vanetza::units::Length;
    using Heading = Angle;
    virtual ~EnvironmentModelObject() = default;

    /**
     * Updates the internal object data.
     */
    virtual void update() = 0;

    /**
     * Returns the external ID of the object.
     * In most cases this is the SUMO identifier.
     * @return external ID
     */
    virtual std::string getExternalId() const = 0;

    /**
     * Returns the polygon describing the object's outline
     * @return polygon points
     */
    virtual const std::vector<Position>& getOutline() const = 0;

    /**
     * Returns a sensor attachment point of the object
     * @param pos logical position of sensor
     * @return sensor attachment point
     */
    virtual const Position& getAttachmentPoint(const SensorPosition& pos) const = 0;

    /**
     * Return the centre point coord of object
     * @return centre point
     */
    virtual const Position& getCentrePoint() const = 0;

    /**
     * Get heading (orientation) of object
     * @return heading as angle from north, clockwise
     */
    virtual Heading getHeading() const = 0;

    /**
     * Get length of object
     */
    virtual Length getLength() const = 0;

    /**
     * Get width of object
     */
    virtual Length getWidth() const = 0;

    /**
     * Return outer object radius
     *
     * Object is guaranteed to lie completely in the circle described by getCentrePoint and getRadius().
     * @return outer radius
     */
    virtual Length getRadius() const = 0;

    /**
     * Returns whether the object should be visible
     * 
     * Vehicles should always be visible. Persons might be driving a vehicle and should not be visibile while doing so.
     */
    virtual bool isVisible() = 0;
};

} // namespace artery

#endif /* EVIRONMENTMODELOBJECT_H_ */
