#ifndef VEHICLE_H_
#define VEHICLE_H_

#include "artery/application/ItsG5Middleware.h"
#include "artery/traci/VehicleController.h"
#include "artery/application/VehicleDataProvider.h"
#include <map>
#include <string>

class StoryboardSignal;
class VehicleDataProvider;

/**
 * Struct containing all necessary vehicle data
 */
class Vehicle
{
public:
    Vehicle(ItsG5Middleware&, std::map<std::string, Vehicle>&);

    const std::string& getId() const;

    traci::VehicleController& getController();
    const traci::VehicleController& getController() const;

    const std::map<std::string, Vehicle>& getVehicles() const;

    void emit(const StoryboardSignal&) const;

    template<typename T>
    const T& get() const
    {
        return mMiddleware.getFacilities()->get_const<T>();
    }

private:
    ItsG5Middleware& mMiddleware;
    std::map<std::string, Vehicle>& mVehicles;
};

#endif /* VEHICLE_H_ */
