#ifndef ORCONDITION_H_
#define ORCONDITION_H_

#include "artery/storyboard/Condition.h"

/**
 * Class to create a Tree of OrConditions
 */
class OrCondition : public Condition
{
public:
    OrCondition(Condition*, Condition*);

    class ResultVisitor : public boost::static_visitor<boost::variant<bool, std::set<Vehicle*>>>
    {
    public:
        ConditionResult operator()(bool lhs, bool rhs) const;
        ConditionResult operator()(std::set<Vehicle*> lhs, bool rhs) const;
        ConditionResult operator()(bool lhs, std::set<Vehicle*> rhs) const;
        ConditionResult operator()(std::set<Vehicle*> lhs, std::set<Vehicle*> rhs) const;
    };

    /**
     * Tests if the condition is true for one car
     * \param Car to test
     * \return true if condition test for left or right is passed
     */
    boost::variant<bool, std::set<Vehicle*>> testCondition(const Vehicle& car);

    void drawCondition(omnetpp::cCanvas*) override;

private:
    Condition* m_left;
    Condition* m_right;
};

#endif /* ORCONDITION_H_ */
