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

    class ResultVisitor : public boost::static_visitor<ConditionResult>
    {
    public:
        ConditionResult operator()(bool lhs, bool rhs) const;
        ConditionResult operator()(std::set<const Vehicle*> lhs, bool rhs) const;
        ConditionResult operator()(bool lhs, std::set<const Vehicle*> rhs) const;
        ConditionResult operator()(std::set<const Vehicle*> lhs, std::set<const Vehicle*> rhs) const;
    };

    /**
     * Tests if the condition is true for one car
     * \param Car to test
     * \return true if condition test for left or right is passed
     */
    ConditionResult testCondition(const Vehicle& car) override;

    void drawCondition(omnetpp::cCanvas*) override;

private:
    Condition* m_left;
    Condition* m_right;
};

#endif /* ORCONDITION_H_ */
