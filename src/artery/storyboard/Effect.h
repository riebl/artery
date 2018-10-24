#ifndef ARTERY_EFFECT_H_
#define ARTERY_EFFECT_H_

namespace artery
{

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

} // namespace artery

#endif /* ARTERY_EFFECT_H_ */
