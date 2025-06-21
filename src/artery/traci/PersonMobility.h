#ifndef ARTERY_PERSONMOBILITY_H_QSO2ZBKC
#define ARTERY_PERSONMOBILITY_H_QSO2ZBKC

#include "artery/traci/ControllablePerson.h"
#include "artery/traci/MobilityBase.h"
#include "traci/PersonSink.h"
#include "traci/VariableCache.h"
#include <string>

namespace artery
{

class PersonMobility :
    public virtual MobilityBase,
    public traci::PersonSink, // for receiving updates from TraCI
    public ControllablePerson // for controlling the person via TraCI
{
public:
    // traci::PersonSink interface
    void initializeSink(std::shared_ptr<traci::API>, std::shared_ptr<traci::PersonCache>, const traci::Boundary&) override;
    void initializePerson(const traci::TraCIPosition&, traci::TraCIAngle, double speed) override;
    void updatePerson(const traci::TraCIPosition&, traci::TraCIAngle, double speed) override;

    const std::string& getId() const { return mPersonId; };

    // ControllablePerson
    traci::PersonController* getPersonController() override;

protected:
    std::string mPersonId;
    std::unique_ptr<traci::PersonController> mController;
};

} // namespace artery

#endif /* ARTERY_PERSONMOBILITY_H_QSO2ZBKC */
