/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef TRACIENVIRONMENTMODELOBJECT_H_
#define TRACIENVIRONMENTMODELOBJECT_H_

#include "artery/application/VehicleDataProvider.h"
#include "artery/envmod/BaseEnvironmentModelObject.h"

// forward declarations
namespace traci
{
    class Controller;
} // namespace traci


namespace artery
{

class TraCIEnvironmentModelObject : public BaseEnvironmentModelObject, private VehicleDataProvider
{
public:
    /**
     * @param ctrl associated TraCI controller to this object
     * @param id station ID used by this object for application messages (e.g. CAM)
     */
    TraCIEnvironmentModelObject(const traci::Controller*, uint32_t id);

    /**
     * Get access to vehicle data provider for this object.
     */
    const VehicleDataProvider& getVehicleData() const;

    void update() override;
    Heading getHeading() const override;
    std::string getExternalId() const override;
    bool isVisible() override;

private:

    const traci::Controller* mController;
};

} // namespace artery

#endif /* TRACIENVIRONMENTMODELOBJECT_H_ */
