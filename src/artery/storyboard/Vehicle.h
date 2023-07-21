#ifndef ARTERY_VEHICLE_H_
#define ARTERY_VEHICLE_H_

#include "artery/application/Middleware.h"
#include "artery/storyboard/Macros.h"
#include "artery/traci/VehicleController.h"
#include "artery/application/VehicleDataProvider.h"
#include <map>
#include <string>

namespace artery
{

class StoryboardSignal;
class VehicleDataProvider;

/**
 * Struct containing all necessary vehicle data
 */
class STORYBOARD_API Vehicle
{
public:
    Vehicle(artery::Middleware&, std::map<std::string, Vehicle>&);

    const std::string& getId() const;

    traci::VehicleController& getController();
    const traci::VehicleController& getController() const;

    const std::map<std::string, Vehicle>& getVehicles() const;

    void emit(const StoryboardSignal&) const;

    template<typename T>
    const T& get() const
    {
        return mMiddleware.getFacilities().get_const<T>();
    }

private:
    artery::Middleware& mMiddleware;
    std::map<std::string, Vehicle>& mVehicles;
};

} // namespace artery

#endif /* ARTERY_VEHICLE_H_ */
