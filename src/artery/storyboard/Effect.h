#ifndef EFFECT_H_
#define EFFECT_H_

namespace Veins
{
class TraCIMobility;
}
class Story;

/**
 * Effect Interface
 */
class Effect
{
public:
    Effect(Story* story, Veins::TraCIMobility& car);
    virtual ~Effect() = default;
    virtual void applyEffect() = 0;
    virtual void reapplyEffect() = 0;
    virtual void removeEffect() = 0;
    Story* getStory();
    Veins::TraCIMobility* getCar();

private:
    Story* m_story;
    Veins::TraCIMobility* m_car;
};

#endif /* EFFECT_H_ */
