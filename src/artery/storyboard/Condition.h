#ifndef _CONDITION_H_
#define _CONDITION_H_

namespace Veins {
    class TraCIMobility;
}

/**
 * Condition Interface
 */
class Condition
{
public:
    virtual ~Condition() = default;
    virtual bool testCondition(Veins::TraCIMobility* car) = 0;
};

#endif  /* CONDITION_H */
