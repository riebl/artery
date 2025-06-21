#ifndef PERSONCONTROLLER_H_AXBS5NQM
#define PERSONCONTROLLER_H_AXBS5NQM

#include "artery/traci/Controller.h"

namespace traci
{

class PersonCache;

class PersonController : public Controller
{
public:
    PersonController(std::shared_ptr<traci::API>, const std::string& id);
    PersonController(std::shared_ptr<traci::API>, std::shared_ptr<PersonCache> cache);

    const std::string& getPersonId() const;
    void setSpeed(Velocity);
    const std::string getVehicle() const;
    const bool isDriving() const;
};

} // namespace traci

#endif /* PERSONCONTROLLER_H_AXBS5NQM */

