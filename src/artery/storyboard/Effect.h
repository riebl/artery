#ifndef EFFECT_H_
#define EFFECT_H_

class Story;
class Vehicle;

/**
 * Effect Interface
 */
class Effect
{
public:
    Effect(Story& story, Vehicle& car);
    virtual ~Effect() = default;
    virtual void applyEffect() = 0;
    virtual void reapplyEffect() = 0;
    virtual void removeEffect() = 0;
    Story& getStory();
    Vehicle& getCar();

private:
    Story& m_story;
    Vehicle& m_car;
};

#endif /* EFFECT_H_ */
