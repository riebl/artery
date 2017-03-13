#ifndef ANDCONDITION_H_
#define ANDCONDITION_H_

#include "artery/storyboard/Condition.h"
#include <algorithm>

/**
 * Class to create a Tree of AndConditions
 */
class AndCondition : public Condition
{
public:
    AndCondition(Condition*, Condition*);

    class ResultVisitor : public boost::static_visitor<boost::variant<bool, std::set<Vehicle*>>>
    {
    public:
        boost::variant<bool, std::set<Vehicle*>> operator()(bool lhs, bool rhs) const;
        boost::variant<bool, std::set<Vehicle*>> operator()(std::set<Vehicle*> lhs, bool rhs) const;
        boost::variant<bool, std::set<Vehicle*>> operator()(bool lhs, std::set<Vehicle*> rhs) const;
        boost::variant<bool, std::set<Vehicle*>> operator()(std::set<Vehicle*> lhs, std::set<Vehicle*> rhs) const;
    };

    /**
     * Tests if the condition is true for one car
     * \param Car to test
     * \return true if both condition tests are passed
     */
    ConditionResult testCondition(const Vehicle& car);

    void drawCondition(omnetpp::cCanvas*) override;

private:
    Condition* m_left;
    Condition* m_right;
};

#endif /* ANDCONDITION_H_ */
