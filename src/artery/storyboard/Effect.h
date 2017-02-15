#ifndef EFFECT_H_
#define EFFECT_H_

class Story;
namespace traci { class VehicleController; }

/**
 * Effect Interface
 */
class Effect
{
public:
    Effect(Story* story, traci::VehicleController& car);
    virtual ~Effect() = default;
    virtual void applyEffect() = 0;
    virtual void reapplyEffect() = 0;
    virtual void removeEffect() = 0;
    Story* getStory();
    traci::VehicleController* getCar();

private:
    Story* m_story;
    traci::VehicleController* m_car;
};

#endif /* EFFECT_H_ */
