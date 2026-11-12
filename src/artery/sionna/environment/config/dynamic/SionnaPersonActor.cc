#include "SionnaPersonActor.h"

#include <artery/sionna/environment/Compat.h>

using namespace artery::sionna;

Define_Module(SionnaPersonActor);

void SionnaPersonActor::initialize(int stage)
{
    InetPersonMobility::initialize(stage);
}

void SionnaPersonActor::finish()
{
    removeSionnaObject(mPersonId);
    InetPersonMobility::finish();
}

const std::string& SionnaPersonActor::sumoId() const
{
    return mPersonId;
}

void SionnaPersonActor::initializePerson(const traci::TraCIPosition& position, traci::TraCIAngle heading, double speed)
{
    initializeSionnaActor(this);
    object() = makeSceneObject(mPersonId);
    auto transform = api()->dynamicConfiguration()->addObject(object());
    updateSionnaObject(transform.get(), position, heading, false);
    PersonMobility::initializePerson(position, heading, speed);
}

void SionnaPersonActor::updatePerson(const traci::TraCIPosition& position, traci::TraCIAngle heading, double speed)
{
    auto transform = api()->dynamicConfiguration()->updateObject(sceneId(mPersonId));
    updateSionnaObject(transform.get(), position, heading);
    PersonMobility::updatePerson(position, heading, speed);
}
