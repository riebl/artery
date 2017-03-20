#include "artery/storyboard/AndCondition.h"

AndCondition::AndCondition(Condition* left, Condition* right) :
    m_left(left), m_right(right)
{
}

ConditionResult AndCondition::ResultVisitor::operator()(bool lhs, bool rhs) const
{
    return (lhs && rhs);
}

ConditionResult AndCondition::ResultVisitor::operator()(std::set<const Vehicle*> lhs, bool rhs) const
{
    if(!rhs) {
        return false;
    } else {
        return lhs;
    }
}

ConditionResult AndCondition::ResultVisitor::operator()(bool lhs, std::set<const Vehicle*> rhs) const
{
    if(!lhs) {
        return false;
    } else {
        return rhs;
    }
}

ConditionResult AndCondition::ResultVisitor::operator()(std::set<const Vehicle*> lhs, std::set<const Vehicle*> rhs) const
{
     std::set<const Vehicle*> intersect;
     std::set_intersection(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), std::inserter(intersect, intersect.begin()));
     return intersect;
}


ConditionResult AndCondition::testCondition(const Vehicle& car)
{
    ConditionResult lhs = m_left->testCondition(car);
    ConditionResult rhs = m_right->testCondition(car);

    return boost::apply_visitor(AndCondition::ResultVisitor(), lhs, rhs);
}

void AndCondition::drawCondition(omnetpp::cCanvas* canvas)
{
    m_left->drawCondition(canvas);
    m_right->drawCondition(canvas);
}
